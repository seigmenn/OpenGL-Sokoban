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

void keyInput(GLFWwindow* window, bool& pressed, bool& wireframeMode, PerspectiveCamera* camera,
              glm::vec2 &selectorPos, std::vector<glm::vec3>& gridPos, int & texture, std::vector<int> & chessPiecePos, int &selectedPiece);

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

FinalApplication::FinalApplication(const std::string& name, const std::string& version) {
    std::cout << "\tName: " << name << "\n\tVersion: " << version << std::endl;
}
FinalApplication::~FinalApplication() {

}

unsigned int FinalApplication::Init() {
    width = 800;
    height = 600;
    GLFWApplication::Init();
    return 0;
}

// Run function

unsigned int FinalApplication::Run() const{

    //NB TEMP LØSNING
    const std::string TEXTURES_DIR = "resources/textures/";

    bool running = true;

    //Enable detailed messages for debug
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(MessageCallback, 0);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

    glm::vec4 player1Col = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
    glm::vec4 player2Col = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    glm::vec4 selectorCol = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
    glm::vec4 selectedCol = glm::vec4(1.0f,1.0f,0.3f,1.0f);
    std::vector<int> chessPiecePos;
    std::vector<glm::vec3> gridPos;
    float X = 8;	//X size of chessboard
    float Y = 8;	//Y size of chessboard

    float offset = 0.37f;
    //Set position of pieces
    for(int i = 0; i < X; i++){
        for(int j = 0; j < Y; j++){
            float squareSize = 0.125f; // size of each square in world units
            float x = (i - 0.5f) * squareSize - offset; // calculate x position of square
            float y = 0.051f; // y position of piece is 0.051 (cube size / 2 + offset off 0.001)
            float z = (j - 0.5f) * squareSize - offset; // calculate z position of square
            gridPos.push_back(glm::vec3(x,y,z));
            if(i == 0 || i == 1 || i == 6 || i == 7){
                chessPiecePos.push_back(gridPos.size()-1);
            }
        }
    }



    auto chessSquare = GeometricTools::UnitGridGeometry2D(X, Y);
    auto chessIndices = GeometricTools::ChessIndices(X, Y);
    auto chessPieceVertices = GeometricTools::createChessPieceVertices(0.1);
    auto chessPieceIndices = GeometricTools::createChessPieceIndices();

    auto selector = GeometricTools::UnitSquare2D;
    std::vector<unsigned int> selectorIndices = {
            0,1,2,
            2,3,0
    };
    //ChessBoard
    std::shared_ptr<VertexBuffer> ChessVB = std::make_shared<VertexBuffer>(chessSquare.data(),chessSquare.size()*sizeof(chessSquare[0]));
    std::shared_ptr<IndexBuffer> ChessIB = std::make_shared<IndexBuffer>(chessIndices.data(), chessIndices.size());
    std::shared_ptr<VertexArray> ChessVA = std::make_shared<VertexArray>();
    //Chesspieces
    std::shared_ptr<VertexBuffer> chessPieceVB = std::make_shared<VertexBuffer>(chessPieceVertices.data(), chessPieceVertices.size() * sizeof(chessPieceVertices[0]));
    std::shared_ptr<IndexBuffer> chessPieceIB = std::make_shared<IndexBuffer>(chessPieceIndices.data(), chessPieceIndices.size());
    std::shared_ptr<VertexArray> chessPieceVA = std::make_shared<VertexArray>();



    auto gridBufferLayout = BufferLayout({
                                                 {ShaderDataType::Float2, "position"}
                                         });
    auto chessPieceLayout = BufferLayout({
                                                 {ShaderDataType::Float3, "position"}
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

    glm::vec2 selectorPos = {0,0};

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

    TextureManager* textureManager = TextureManager::GetInstance();
    textureManager->LoadTexture2DRGBA("floor", std::string(TEXTURES_DIR) + std::string("floor_texture.png"),0);
    textureManager->LoadCubeMapRGBA("cube", std::string(TEXTURES_DIR) + std::string("cube_texture.png"),1);

    PerspectiveCamera* camera = new PerspectiveCamera();

    camera->SetLookAt(glm::vec3(0.0f));


    glEnable(GL_DEPTH_TEST);
    bool wireframeMode = false;
    int drawTexture = 0;
    bool pressed = false;
    int selectedPiece = -1;
    do
    {
        RenderCommands::Clear();

        glm::vec2 tempPos = { selectorPos.x , selectorPos.y };

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
        chessBoardShader->SetUniform2f("u_selectorPos", tempPos);

        ChessVA->Bind();
        RenderCommands::DrawIndex(ChessVA, GL_TRIANGLES);
        ChessVA->Unbind();

        //Draw cubes:
        chessPieceShader->use();


        for(int i = 0; i < chessPiecePos.size(); i++) {
            chessPieceVA->Bind();
            if (i < 16) {
                chessPieceShader->SetUniform4f("u_cubeColor", player1Col);
            } else {
                chessPieceShader->SetUniform4f("u_cubeColor", player2Col);
            }
            int selectorIndex = selectorPos.y + selectorPos.x * 8;
            if (selectorIndex == chessPiecePos[i]){
                chessPieceShader->SetUniform4f("u_cubeColor", selectorCol);
            }
            if (i == selectedPiece){
                chessPieceShader->SetUniform4f("u_cubeColor", selectedCol);
            }

            chessPieceShader->setInt("u_texture",drawTexture);

            rotationAngle = 0.0f; // Negative angle to look down

            // Create the translation matrix
            translationMatrix = glm::translate(glm::mat4(1.0f), gridPos[chessPiecePos[i]]);
            rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(rotationAngle), glm::vec3(1.0f, 0.0f, 0.0f));

            scaleFactor = 0.4;
            scaleMatrix = glm::scale(glm::mat4(1.0f),glm::vec3(scaleFactor));

            // Combine the translation, rotation and scale
            model = translationMatrix * rotationMatrix * scaleMatrix;

            chessPieceShader->setMat4("u_pieceModMat", model);
            chessPieceShader->setMat4("u_pieceViewProjMat", camera->GetViewProjectionMatrix());

            RenderCommands::DrawIndex(chessPieceVA, GL_TRIANGLES);
            chessPieceVA->Unbind();
        }

        keyInput(window, pressed, wireframeMode, camera, selectorPos, gridPos,drawTexture, chessPiecePos, selectedPiece);

        glfwWaitEventsTimeout(0.2);
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
 *  keyInput
 * **/
void keyInput(GLFWwindow* window, bool& pressed, bool& wireframeMode, PerspectiveCamera* camera,
              glm::vec2 &selectorPos, std::vector<glm::vec3>& gridPos, int &texture, std::vector<int> &chessPiecePos, int &selectedPiece){
    float angle = 0.1f;

    if((glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) && !pressed){

        if (wireframeMode)
            RenderCommands::SetSolidMode();
        else
            RenderCommands::SetWireframeMode();

        wireframeMode = !wireframeMode;
    }

    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
    {
        camera->RotateCamera(angle);
        //auto pos = camera->GetPosition();
        //std::cout << pos.x << "\t" << pos.y << "\t" << pos.z << "\n";
    }
    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
    {
        camera->RotateCamera(-angle);
        //auto pos = camera->GetPosition();
        //std::cout << pos.x << "\t" << pos.y << "\t" << pos.z << "\n";
    }
    float zoomSpeed = 0.1f;
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
        // Zoom in by moving the camera closer to the LookAt point
        camera->ZoomCamera(true,zoomSpeed);
    }

    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
        // Zoom out by moving the camera away from the LookAt point
        camera->ZoomCamera(false,zoomSpeed);
    }
    //Move selector around the board
    if ((glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)) {
        if(selectorPos.y < 7)
            selectorPos.y++;
    }

    if ((glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)) {
        if(selectorPos.y > 0)
            selectorPos.y--;
    }

    if ((glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)) {
        if(selectorPos.x < 7)
            selectorPos.x++;
    }

    if ((glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)) {
        if(selectorPos.x > 0)
            selectorPos.x--;
    }
    //Select a piece
    if ((glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)){
        //Convert coordinates to index (0-63)
        int selectorIndex = selectorPos.y + selectorPos.x * 8;
        //If a piece is not already selected:
        if(selectedPiece == -1) {
            //Find which piece is on this coordinate
            for (int i = 0; i < chessPiecePos.size(); i++) {
                if (selectorIndex == chessPiecePos[i]) {
                    //Set piece to selected
                    selectedPiece = i;
                }
            }
        } else{
            bool occupied = false;
            for (int chessPiecePo : chessPiecePos){
                if (selectorIndex == chessPiecePo){ //If there is a piece on the selectors coordinate
                    occupied = true;
                }
            }
            if (!occupied){ //Only move if there is not a piece there
                chessPiecePos[selectedPiece] = selectorIndex;
            }
            selectedPiece = -1;
        }
    }


    if ((glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)) {
        //PRINT DEBUG INFO
    }

    if ((glfwGetKey(window,GLFW_KEY_T) == GLFW_PRESS)) {
        texture = static_cast<bool>(!texture);
    }
}



