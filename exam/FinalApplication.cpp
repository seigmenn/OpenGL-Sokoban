//
// Created by auror on 15/12/2023.
//
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <string>
#include <algorithm>
#include <memory>
#include <cstring>
#include <glm/glm.hpp>
#include "FinalApplication.h"
#include "../../group_24/framework/Rendering/Shader.h"
#include "../../group_24/framework/GeometricTools/GeometricTools.h"
#include "../../group_24/framework/Rendering/IndexBuffer.h"
#include "../../group_24/framework/Rendering/VertexBuffer.h"
#include "../../group_24/framework/Rendering/BufferLayout.h"
#include "../../group_24/framework/Rendering/VertexArray.h"
#include "../../group_24/framework/Rendering/RenderCommands.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include "../../group_24/framework/Rendering/PerspectiveCamera.h"
#include "shaders.h"
#include "../../group_24/framework/Rendering/OrthographicCamera.h"
#include "../../group_24/framework/Rendering/TextureManager.h"
#include "iomanip"
#include <glm/gtc/type_ptr.hpp>

void keyInput(GLFWwindow* window, PerspectiveCamera* camera,
              glm::vec2 &currentPlayerPos, std::vector<glm::vec3>& gridPos, int & texture, std::vector<int>& movablePieces, const std::vector<int>& pillarPositions
        ,const std::vector<int>& targetPositions);

// -----------------------------------------------------------------------------
//  MESSAGE CALLBACK
// -----------------------------------------------------------------------------
void GLAPIENTRY MessageCallback(GLenum source,
GLenum type,
        GLuint id,
GLenum severity,
        GLsizei length,
const GLchar* message,
const void* userParam)
{
// Outputs debugging messages from OpenGL
std::cerr << "GL CALLBACK: "
<< (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "")
<< " type = 0x" << type
<< ", severity = 0x" << severity
<< ", message = " << message << "\n";
}

// -----------------------------------------------------------------------------
//  GLFW ERROR CALLBACK
// -----------------------------------------------------------------------------

/**
 * moveBox moves the box in the direction the player wants to move
 * @param movablePiece the position of the movable piece
 * @param playerPos the position of the player
 * @param direction the direction the player wants to move
 *
 * @return the new position of the movable piece
 * */
int moveBox(int movablePiece, glm::vec2 playerPos, int direction){
    glm::vec2 newPosition = playerPos;
    switch(direction){
        case 1: //Down
            newPosition.y += 2;
            break;
        case 2: //Up
            newPosition.y -= 2;
            break;
        case 3: //Left
            newPosition.x -= 2;
            break;
        case 4: //Right
            newPosition.x += 2;
            break;
        default:
             break;
    }
   movablePiece = newPosition.x*10+newPosition.y;
    return movablePiece;

}
/**
 *  checkOccupied checks if the new position the player wants to move to is occupied
 *  @param currentPlayerPos the current position of the player
 *  @param direction the direction the player wants to move
 *  @param gridPos the entire chessboard grid
 *  @param movablePieces the position of the movable pieces
 *  @param pillarPositions the position of the pillars
 *  @param targetPositions the position of the targets
 *
 *  @return true if the new position is occupied, false if it is free
 * */
 bool checkOccupied(glm::vec2& currentPlayerPos, int direction, const std::vector<glm::vec3>& gridPos, std::vector<int>& movablePieces, const std::vector<int>& pillarPositions,
                    const std::vector<int>& targetPositions) {
    int gridSize = 10;

    // Calculate the new position based on the direction
    glm::vec2 newPosition = currentPlayerPos;
    switch (direction) {
        case 1: // Down
            newPosition.y += 1;
            break;
        case 2: // Up
            newPosition.y -= 1;
            break;
        case 3: // Left
            newPosition.x -= 1;
            break;
        case 4: // Right
            newPosition.x += 1;
            break;
        default:
            return false;
    }

    if (newPosition.x < 0 || newPosition.x >= (gridSize) || newPosition.y < 0 || newPosition.y >= (gridSize)) {
        return true; //Collision with wall
    }

    // Check for collisions with pillars
    for (int j = 0; j < pillarPositions.size(); j++) {
        int pillarIndex = pillarPositions[j];
        if (gridPos[newPosition.x * gridSize + newPosition.y] == gridPos[pillarIndex]) {
            return true; // Collision with a pillar
        }
    }

    // Check for collisions with movable pieces
    for (int i = 0; i < movablePieces.size(); i++) {
        int movableIndex = movablePieces[i];
        if (gridPos[newPosition.x * gridSize + newPosition.y] == gridPos[movableIndex]) {
            // Check if the chessboard piece behind the movable piece is free by determining the new position and then
            //rechecking surrounding pillars, movable pieces and out of bounds
            glm::vec2 behindMovable = newPosition;
            switch (direction) {
                case 1: // Down
                    behindMovable.y += 1;
                    break;
                case 2: // Up
                    behindMovable.y -= 1;
                    break;
                case 3: // Left
                    behindMovable.x -= 1;
                    break;
                case 4: // Right
                    behindMovable.x += 1;
                    break;
            }
            for (int j = 0; j < pillarPositions.size(); j++) {
                int pillarIndex = pillarPositions[j];
                if (gridPos[behindMovable.x * gridSize + behindMovable.y] == gridPos[pillarIndex]) {
                    return true;
                }
            }
            for(int k=0; k<movablePieces.size();k++){
                movableIndex = movablePieces[k];
                if(gridPos[behindMovable.x*gridSize+behindMovable.y] == gridPos[movableIndex]){
                    return true;
                }
            }
            if(behindMovable.x < 1 || behindMovable.x >= (gridSize-1) || behindMovable.y < 1 || behindMovable.y >= (gridSize-1)){
                return true;
            }
            else{
                movablePieces[i] = moveBox(movablePieces[i], currentPlayerPos, direction);
                return false;
            }
        }
    }
    // No collision detected
    return false;
}

FinalApplication::FinalApplication(const std::string& name, const std::string& version) {
    std::cout << "\tName: " << name << "\n\tVersion: " << version << std::endl;
}
FinalApplication::~FinalApplication() {

}

/**
 *  Initialization of the GLFW window
 *  @return 0 if success
 * */
unsigned int FinalApplication::Init() {
    width = 1024;
    height = 1024;
    GLFWApplication::Init();
    return 0;
}


/**
 *  Run function containing the main loop of the application
 *  @return 0 if success
 * */
unsigned int FinalApplication::Run() const{

    const std::string TEXTURES_DIR = "resources/textures/";
    bool running = true;

    //Enable detailed messages for debug
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(MessageCallback, 0);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

    glm::vec4 player1Col = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
    glm::vec4 player2Col = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

    //Setting the grid of the chessboard which everything else is placed within
    std::vector<glm::vec3> gridPos;
    float X = 10;	//X size of chessboard
    float Y = 10;	//Y size of chessboard

    float offset = 0.4f;
    float squareSize = 0.1f; // size of each square in world units

// Set position of wall
    std::vector<int> chessPiecePos;
    for (int i = 0; i < X; i++) {
        for (int j = 0; j < Y; j++) {
            float x = (i - 0.5f) * squareSize - offset; // calculate x position of square
            float y = 0.02f; // y position of piece is 0.051 (cube size / 2 + offset off 0.001)
            float z = (j - 0.5f) * squareSize - offset; // calculate z position of square
            gridPos.push_back(glm::vec3(x, y, z));
            // Position the pieces in a wall around the board
            if (i == 0 || i == 9 || j == 0 || j == 9) {
                chessPiecePos.push_back(gridPos.size() - 1);
            }
        }
    }

    //Create chessboard and its indices and chessboard pieces (cubes) and their indices
    auto chessSquare = GeometricTools::UnitGridGeometry2D(X, Y);
    auto chessIndices = GeometricTools::ChessIndices(X, Y);
    auto chessPieceVertices = GeometricTools::createChessPieceVertices(squareSize);
    auto chessPieceIndices = GeometricTools::createChessPieceIndices();

    //Creating a random start position for the player within the chessboard
    glm::vec2 playerPos = {
            static_cast<float>(rand() % 8 +1),
            static_cast<float>(rand() % 8+1)
    };

    //Pillars
    std::vector<glm::vec3> pillarPos;
    std::vector<int> pillarPositions;
    for (int i = 0; i < 6; i++) {
        auto posx = static_cast<float>(rand() % 8 + 1);
        auto posz = static_cast<float>(rand() % 8 + 1);

        // Check if the pillar position collides with existing pillars or the player
        if (std::find(pillarPositions.begin(), pillarPositions.end(), posx * X + posz) != pillarPositions.end() ||
            (posx == playerPos.x && posz == playerPos.y)) {
            i--;  // Retry with a new position
            continue;
        }

        pillarPositions.push_back(posx * X + posz);
    }

    //Movable pieces/boxes
    std::vector<int> movablePieces;
    for (int i = 0; i < 6; i++) {
        auto posx = (rand() % 8 + 1) * X;
        auto posz = (rand() % 8 + 1);

        // Check if the movable piece position collides with pillars, the player, or other movable pieces
        if (std::find(pillarPositions.begin(), pillarPositions.end(), posx + posz) != pillarPositions.end() ||
            (posx + posz == playerPos.x * X + playerPos.y) ||
            std::find(movablePieces.begin(), movablePieces.end(), posx + posz) != movablePieces.end()) {
            i--;
            continue;
        }

        movablePieces.push_back(posx + posz);
    }

    //Targets
    std::vector<int> targetPositions;
    for (int i = 0; i < 6; i++) {
        auto posx = (rand() % 8 + 1) * X;
        auto posz = (rand() % 8 + 1);

        // Check if the target position collides with pillars, the player, movable pieces, or other targets
        if (std::find(pillarPositions.begin(), pillarPositions.end(), posx + posz) != pillarPositions.end() ||
            (posx + posz == playerPos.x * X + playerPos.y) ||
            std::find(movablePieces.begin(), movablePieces.end(), posx + posz) != movablePieces.end() ||
            std::find(targetPositions.begin(), targetPositions.end(), posx + posz) != targetPositions.end()) {
            i--;
            continue;
        }

        targetPositions.push_back(posx + posz);
    }

    //Sun for task 5
    glm::vec3 sunPos = glm::vec3(X * squareSize*0.5f-offset, 1.0f, Y * squareSize * 0.5f-offset);

    //ChessBoard
    std::shared_ptr<VertexBuffer> ChessVB = std::make_shared<VertexBuffer>(chessSquare.data(),chessSquare.size()*sizeof(chessSquare[0]));
    std::shared_ptr<IndexBuffer> ChessIB = std::make_shared<IndexBuffer>(chessIndices.data(), chessIndices.size());
    std::shared_ptr<VertexArray> ChessVA = std::make_shared<VertexArray>();
    //Cubes
    std::shared_ptr<VertexBuffer> chessPieceVB = std::make_shared<VertexBuffer>(chessPieceVertices.data(), chessPieceVertices.size() * sizeof(chessPieceVertices[0]));
    std::shared_ptr<IndexBuffer> chessPieceIB = std::make_shared<IndexBuffer>(chessPieceIndices.data(), chessPieceIndices.size());
    std::shared_ptr<VertexArray> chessPieceVA = std::make_shared<VertexArray>();

    auto gridBufferLayout = BufferLayout({
                                                 {ShaderDataType::Float2, "position"}
                                         });
    auto chessPieceLayout = BufferLayout({
                                                 {ShaderDataType::Float3, "position"},
                                                 {ShaderDataType::Float3, "normals"}
                                         });

    ChessVA->Bind();
    ChessVB->SetLayout(gridBufferLayout);
    ChessVA->AddVertexBuffer(ChessVB);
    ChessVA->SetIndexBuffer(ChessIB);
    ChessVA->Unbind();

    chessPieceVA->Bind();
    chessPieceVB->SetLayout(chessPieceLayout);
    chessPieceVA->AddVertexBuffer(chessPieceVB);
    chessPieceVA->SetIndexBuffer(chessPieceIB);
    chessPieceVA->Unbind();

    //Creating shaders
    std::shared_ptr<Shader> chessBoardShader = std::make_shared<Shader>(ChessBoardVertexShader, ChessBoardFragmentShader);
    std::shared_ptr<Shader> chessPieceShader = std::make_shared<Shader>(ChessPieceVertexShader, ChessPieceFragmentShader);

    chessBoardShader->use();
    glm::vec2 gridSize = { X,Y };
    chessBoardShader->SetUniform2f("u_divisions", gridSize);

    glm::vec4 bColor = glm::vec4(0.2f,0.2f,0.2f,1.0f);
    RenderCommands::SetClearColor(bColor);   // background colour
    if (!window) {
        fprintf(stderr, "Failed to open window\n");
        return -1;
    }

    //Loading textures for floor and cubes/walls
    TextureManager* textureManager = TextureManager::GetInstance();
    textureManager->LoadTexture2DRGBA("floor", std::string(TEXTURES_DIR) + std::string("floor_texture.png"),0);
    textureManager->LoadCubeMapRGBA("cube", std::string(TEXTURES_DIR) + std::string("cube_texture.png"),1);

    PerspectiveCamera* camera = new PerspectiveCamera();

    camera->SetLookAt(glm::vec3(0.0f));
    glm::vec2 currentPlayerPos = { playerPos.x, playerPos.y};

    //Setting a global lightPosition/lightColor
    glm::vec3 lightPosition = glm::vec3(1.2f, 1.0f, 2.0f);
    glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);

    glEnable(GL_DEPTH_TEST);

    //To determine whether textures should be drawn or not
    int drawTexture = 0;
    int startTime = glfwGetTime();
    do
    {
        RenderCommands::Clear();

        //Time
        double currentTime = glfwGetTime();
        double deltaTime = fmod(currentTime-startTime, 45.0);
        float rotationSun = (2*glm::two_pi<float>()/45.0f)*deltaTime;
        sunPos = glm::vec3(X*squareSize*0.5f-offset, 1.0f, Y*squareSize*0.5f-offset);

        float maxAmbi = 0.3f;
        float minAmbi = 0.1f;

        glm::mat4 rotationSunMatrix = glm::rotate(glm::mat4(1.0f), rotationSun, glm::vec3(0.0f, 0.0f, 1.0f));
        sunPos= rotationSunMatrix*glm::vec4(sunPos,1.0f);

        lightPosition = sunPos;
        float ambientStrength = glm::mix(minAmbi, maxAmbi,glm::clamp(sunPos.y, minAmbi, maxAmbi) );
        float diffuseStrength = glm::mix(0.0f, 1.0f, glm::clamp(sunPos.y, 0.0f, 1.0f));
        float specularStrength = glm::mix(0.0f, 1.0f, glm::clamp(sunPos.y, 0.0f, 1.0f));

        // Define the desired position of the chessboard
        glm::vec3 chessboardPosition = glm::vec3(0.0f, 0.0f, 0.0f); // Adjust as needed
        // Define the rotation angle (in degrees) to look down at the chessboard
        float rotationAngle = 0.0f; // Negative angle to look down
        // Create the translation matrix
        glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), chessboardPosition);
        // Create the rotation matrix to look down
        glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(rotationAngle), glm::vec3(1.0f, 0.0f, 0.0f));
        float scaleFactor = 1;
        glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f),glm::vec3(scaleFactor));
        // Combine the translation and rotation
        glm::mat4 model = translationMatrix * rotationMatrix * scaleMatrix;

        chessBoardShader->use();
        chessBoardShader->setMat4("u_viewProjMat", camera->GetViewProjectionMatrix());
        chessBoardShader->setMat4("u_modMat",model);
        chessBoardShader->setInt("u_texture",drawTexture);
        chessBoardShader->SetUniform3f("u_lightPos", lightPosition);
        chessPieceShader->setFloat("ambientStrength", ambientStrength);
        chessPieceShader->setFloat("diffuseStrength", diffuseStrength);
        chessPieceShader->setFloat("specularStrength", specularStrength);
        chessBoardShader->SetUniform3f("u_lightColor", lightColor);
        chessBoardShader->SetUniform3f("u_viewPos", camera->GetPosition());

        ChessVA->Bind();
        RenderCommands::DrawIndex(ChessVA, GL_TRIANGLES);
        ChessVA->Unbind();

        //Draw walls:
        chessPieceShader->use();
        chessPieceShader->setMat4("u_pieceViewProjMat", camera->GetViewProjectionMatrix());
        chessPieceShader->SetUniform3f("u_lightPos", lightPosition);
        chessPieceShader->SetUniform3f("u_lightColor", lightColor);
        chessPieceShader->setFloat("ambientStrength", ambientStrength);
        chessPieceShader->setFloat("diffuseStrength", diffuseStrength);
        chessPieceShader->setFloat("specularStrength", specularStrength);
        chessPieceShader->SetUniform3f("u_viewPos", camera->GetPosition());
        chessPieceShader->SetUniform4f("u_cubeColor", player2Col);

        for(int i = 0; i < chessPiecePos.size(); i++) {
            chessPieceVA->Bind();


            chessPieceShader->setInt("u_texture",drawTexture);

            rotationAngle = 0.0f; // Negative angle to look down

            // Create the translation matrix
            translationMatrix = glm::translate(glm::mat4(1.0f), gridPos[chessPiecePos[i]]);
            rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(rotationAngle), glm::vec3(1.0f, 0.0f, 0.0f));

            scaleFactor = 0.5f;
            scaleMatrix = glm::scale(glm::mat4(1.0f),glm::vec3(scaleFactor));

            // Combine the translation, rotation and scale
            model = translationMatrix * rotationMatrix * scaleMatrix;

            chessPieceShader->setMat4("u_pieceModMat", model);
            chessPieceShader->setMat4("u_pieceViewProjMat", camera->GetViewProjectionMatrix());

            RenderCommands::DrawIndex(chessPieceVA, GL_TRIANGLES);
            chessPieceVA->Unbind();
        }

        //Draw player cube
        chessPieceShader->use();
        rotationAngle = 0.0f; // Negative angle to look down
        // Create the translation matrix
        translationMatrix = glm::translate(glm::mat4(1.0f), gridPos[currentPlayerPos.x*X+currentPlayerPos.y]);
        rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(rotationAngle), glm::vec3(1.0f, 0.0f, 0.0f));

        scaleFactor = 0.5;
        scaleMatrix = glm::scale(glm::mat4(1.0f),glm::vec3(scaleFactor));
        // Combine the translation, rotation and scale
        model = translationMatrix * rotationMatrix * scaleMatrix;

        chessPieceShader->setMat4("u_pieceViewProjMat", camera->GetViewProjectionMatrix());
        chessPieceShader->setMat4("u_pieceModMat",model);
        chessPieceShader->SetUniform4f("u_cubeColor", glm::vec4(0.92f,1.0f,0.0f,1.0f));

        chessPieceVA->Bind();
        RenderCommands::DrawIndex(chessPieceVA, GL_TRIANGLES);
        chessPieceVA->Unbind();

        //Draw boxes
        for(int i=0; i<movablePieces.size();i++){
           chessPieceShader->use();
            scaleFactor = 0.5;
            scaleMatrix = glm::scale(glm::mat4(1.0f),glm::vec3(scaleFactor));
           translationMatrix = glm::translate(glm::mat4(1.0f), gridPos[movablePieces[i]]);
           model = translationMatrix * rotationMatrix * scaleMatrix;
            chessPieceShader->SetUniform4f("u_cubeColor", glm::vec4(1.0f,0.0f,1.0f,1.0f));

              chessPieceShader->setMat4("u_pieceViewProjMat", camera->GetViewProjectionMatrix());
              chessPieceShader->setMat4("u_pieceModMat",model);
              for(int j=0; j<targetPositions.size();j++){
                  if(movablePieces[i] == targetPositions[j]){
                      chessPieceShader->SetUniform4f("u_cubeColor", glm::vec4(0.0f,1.0f,0.0f,1.0f));
                  }
              }

           chessPieceVA->Bind();
           RenderCommands::DrawIndex(chessPieceVA, GL_TRIANGLES);
              chessPieceVA->Unbind();
        };

        //Draw pillars
        for(int i=0; i<pillarPositions.size(); i++){
            chessPieceShader->use();
            translationMatrix = glm::translate(glm::mat4(1.0f), gridPos[pillarPositions[i]]);
            rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(rotationAngle), glm::vec3(1.0f, 0.0f, 0.0f));


            model = translationMatrix * rotationMatrix * scaleMatrix;

            chessPieceShader->setMat4("u_pieceViewProjMat", camera->GetViewProjectionMatrix());
            chessPieceShader->setMat4("u_pieceModMat",model);
            chessPieceShader->SetUniform4f("u_cubeColor", glm::vec4(0.0f,0.0f,1.0f,1.0f));
            chessPieceShader->setInt("u_texture",drawTexture);

            chessPieceVA->Bind();
            RenderCommands::DrawIndex(chessPieceVA, GL_TRIANGLES);
            chessPieceVA->Unbind();
        }

        //Draw targets
        for(int i=0; i<targetPositions.size();i++){
            chessPieceShader->use();

            chessPieceShader->SetUniform4f("u_cubeColor", glm::vec4(0.0f,1.0f,0.0f,0.2f));

            scaleFactor = 0.3;
            scaleMatrix = glm::scale(glm::mat4(1.0f),glm::vec3(scaleFactor));
            translationMatrix = glm::translate(glm::mat4(1.0f), gridPos[targetPositions[i]]);
            model = translationMatrix * rotationMatrix * scaleMatrix;

            chessPieceShader->setMat4("u_pieceViewProjMat", camera->GetViewProjectionMatrix());
            chessPieceShader->setMat4("u_pieceModMat",model);

            chessPieceVA->Bind();
            RenderCommands::DrawIndex(chessPieceVA, GL_TRIANGLES);
            chessPieceVA->Unbind();
        };

        //Draw sun
        chessPieceShader->use();
        chessPieceShader->SetUniform4f("u_cubeColor", glm::vec4(1.0f,1.0f,1.0f,1.0f));
        chessPieceShader->setFloat("ambientStrength", 1.0f);

        scaleFactor = 0.5;
        scaleMatrix = glm::scale(glm::mat4(1.0f),glm::vec3(scaleFactor));
        translationMatrix = glm::translate(glm::mat4(1.0f), sunPos);
        model = translationMatrix * rotationMatrix * scaleMatrix;

        chessPieceShader->setMat4("u_pieceViewProjMat", camera->GetViewProjectionMatrix());
        chessPieceShader->setMat4("u_pieceModMat",model);

        chessPieceVA->Bind();
        RenderCommands::DrawIndex(chessPieceVA, GL_TRIANGLES);
        chessPieceVA->Unbind();

        keyInput(window, camera, currentPlayerPos, gridPos,drawTexture, movablePieces,pillarPositions, targetPositions);

        glfwWaitEventsTimeout(0.15);
        glfwSwapBuffers(window);
        glfwPollEvents();


        running &= (glfwGetKey(window, GLFW_KEY_Q) == GLFW_RELEASE);
        running &= (glfwWindowShouldClose(window) != GL_TRUE);

    } while (running);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

/**
 *  Handle key input
 *  @param window The window to check for key input
 *  @param camera The camera to move
 *  @param currentPlayerPos The current position of the player
 *  @param gridPos The positions of the chessboard squares
 *  @param texture Whether to draw the texture or not
 *  @param chessPiecePos The positions of the chess pieces
 *  @param selectedPiece The index of the selected chess piece
 *  @param movablePieces The positions of the movable pieces
 *  @param pillarPositions The positions of the pillars
 *  @param targetPositions The positions of the targets
 * **/
void keyInput(GLFWwindow* window, PerspectiveCamera* camera,
              glm::vec2& currentPlayerPos, std::vector<glm::vec3>& gridPos, int &texture, std::vector<int>& movablePieces, const std::vector<int>& pillarPositions
              ,const std::vector<int>& targetPositions){
    float angle = 0.1f;

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        camera->RotateCamera(angle);
        //auto pos = camera->GetPosition();
        //std::cout << pos.x << "\t" << pos.y << "\t" << pos.z << "\n";
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        camera->RotateCamera(-angle);
        //auto pos = camera->GetPosition();
        //std::cout << pos.x << "\t" << pos.y << "\t" << pos.z << "\n";
    }
    float zoomSpeed = 0.1f;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        // Zoom in by moving the camera closer to the LookAt point
        camera->ZoomCamera(true,zoomSpeed);
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        // Zoom out by moving the camera away from the LookAt point
        camera->ZoomCamera(false,zoomSpeed);
    }

    if ((glfwGetKey(window,GLFW_KEY_T) == GLFW_PRESS)) {
        texture = static_cast<bool>(!texture);
    }
    if(glfwGetKey(window,GLFW_KEY_DOWN) == GLFW_PRESS){
        if(currentPlayerPos.y < 8){
        if(!checkOccupied(currentPlayerPos, 1, gridPos, movablePieces, pillarPositions,targetPositions))
            currentPlayerPos.y += 1;
        }
    }
    if(glfwGetKey(window,GLFW_KEY_UP) == GLFW_PRESS){
        if(currentPlayerPos.y > 1){
            if(!checkOccupied(currentPlayerPos, 2, gridPos, movablePieces, pillarPositions,targetPositions))
                currentPlayerPos.y -= 1;
        }
    }
    if(glfwGetKey(window,GLFW_KEY_LEFT) == GLFW_PRESS){
        if( currentPlayerPos.x > 1) {
            if (!checkOccupied(currentPlayerPos, 3, gridPos, movablePieces, pillarPositions, targetPositions))
                currentPlayerPos.x -= 1;
        }
    }
    if(glfwGetKey(window,GLFW_KEY_RIGHT) == GLFW_PRESS){
        if(currentPlayerPos.x < 8){
        if(!checkOccupied(currentPlayerPos, 4, gridPos, movablePieces, pillarPositions ,targetPositions))
            currentPlayerPos.x += 1;
        }
    }
}



