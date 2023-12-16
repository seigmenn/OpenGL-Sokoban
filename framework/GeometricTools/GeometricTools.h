#ifndef __GEOMETRICTOOLS_H
#define __GEOMETRICTOOLS_H

#include <array>
#include <vector>
#include <numeric>

namespace GeometricTools {


	constexpr std::array<float, 3 * 2> UnitTriangle2D =
	{ -0.5f, -0.5f,
		 0.5f, -0.5f,
		 0.0f,  0.5f };


	std::vector<float> UnitSquare2D =
	{   -0.5f, -0.5f,
		-0.5f,  0.5f,
		 0.5f,  0.5f,
		-0.5f, -0.5f,
		 0.5f,  0.5f,
		 0.5f, -0.5f
	};
	//WITH COLOUR:
	constexpr std::array<float, 5 * 4> TestChessSquare3D =
	{
		// positions					// colors
		-0.05f, -0.05f,			1.0f, 0.0f, 0.0f,
		 0.05f, -0.05f,			0.0f, 1.0f, 0.0f,
		 0.05f,  0.05f,			0.0f, 0.0f, 1.0f,
		-0.05f,  0.05f,			0.0f, 1.0f, 1.0f
	};


    /*std::vector<float> createChessPieceVertices(float size)
    {
        float vertices[] = {
                // Front face
                -size, -size,  size,
                size, -size,  size,
                size,  size,  size,
                -size,  size,  size,

                // Back face
                -size, -size, -size,
                -size,  size, -size,
                size,  size, -size,
                size, -size, -size,

                // Left face
                -size,  size,  size,
                -size,  size, -size,
                -size, -size, -size,
                -size, -size,  size,

                // Right face
                size,  size,  size,
                size, -size,  size,
                size, -size, -size,
                size,  size, -size,

                // Top face
                -size,  size,  size,
                -size,  size, -size,
                size,  size, -size,
                size,  size,  size,

                // Bottom face
                -size, -size,  size,
                size, -size,  size,
                size, -size, -size,
                -size, -size, -size
        };

        return std::vector<float>(std::begin(vertices), std::end(vertices));
    }*/

    std::vector<float> createChessPieceVertices(float size)
    {
        float vertices[] = {
                // Front face
                -size, -size, size, 0.0f, 0.0f, 1.0f,
                size, -size, size, 0.0f, 0.0f, 1.0f,
                size, size, size, 0.0f, 0.0f, 1.0f,
                -size, size, size, 0.0f, 0.0f, 1.0f,

                // Back face
                -size, -size, -size, 0.0f, 0.0f, -1.0f,
                -size, size, -size, 0.0f, 0.0f, -1.0f,
                size, size, -size, 0.0f, 0.0f, -1.0f,
                size, -size, -size, 0.0f, 0.0f, -1.0f,

                // Left face
                -size, size, size, -1.0f, 0.0f, 0.0f,
                -size, size, -size, -1.0f, 0.0f, 0.0f,
                -size, -size, -size, -1.0f, 0.0f, 0.0f,
                -size, -size, size, -1.0f, 0.0f, 0.0f,

                // Right face
                size, size, size, 1.0f, 0.0f, 0.0f,
                size, -size, size, 1.0f, 0.0f, 0.0f,
                size, -size, -size, 1.0f, 0.0f, 0.0f,
                size, size, -size, 1.0f, 0.0f, 0.0f,

                // Top face
                -size, size, size, 0.0f, 1.0f, 0.0f,
                -size, size, -size, 0.0f, 1.0f, 0.0f,
                size, size, -size, 0.0f, 1.0f, 0.0f,
                size, size, size, 0.0f, 1.0f, 0.0f,

                // Bottom face
                -size, -size, size, 0.0f, -1.0f, 0.0f,
                size, -size, size, 0.0f, -1.0f, 0.0f,
                size, -size, -size, 0.0f, -1.0f, 0.0f,
                -size, -size, -size, 0.0f, -1.0f, 0.0f
        };

        return std::vector<float>(std::begin(vertices), std::end(vertices));
    }

    std::vector<GLuint> cubeTopologyWNormals = {
            2,5,8,		5,8,11,		//Front
            6,9,12,		9,12,15,	//Right
            14,17,20,	17,20,23,	//Back
            0,3,18,		3,18,21,	//Left
            4,10,16,	4,16,22,	//Top
            1,7,13,		1,13,19		//Bottom

    };
    constexpr std::array<float, 3 * 24 * 2> UnitCube3D24WNormals = {
            -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,// Left - Bottom - Front
            -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,// Left - Top - Front
            -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

            0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,// Right - Bottom - Front
            0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

            0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,// Right - Top - Front
            0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

            0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,// Right - Bottom - Back
            0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
            0.5f, -0.5f, -0.5f,  0.0f,  0.0f,  -1.0f,

            0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,// Right - Top - Back
            0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  0.0f,  -1.0f,

            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,// Left - Bottom - Back
            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f,  -1.0f,

            -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,// Left - Top - Back
            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  0.0f,  -1.0f
    };

    constexpr std::array<float, 3 * 2 * 2 * 2> UnitCube3D = {
            -0.5f, -0.5f,  0.5f, // Left - Bottom - Front
            -0.5f,  0.5f,  0.5f, // Left - Top - Front
            0.5f, -0.5f,  0.5f, // Right - Bottom - Front
            0.5f,  0.5f,  0.5f, // Right - Top - Front
            0.5f, -0.5f, -0.5f, // Right - Bottom - Back
            0.5f,  0.5f, -0.5f, // Right - Top - Back
            -0.5f, -0.5f, -0.5f, // Left - Bottom - Back
            -0.5f,  0.5f, -0.5f // Left - Top - Back
    };

    std::vector<GLuint> cubeTopology = {
            0,1,2,	//front
            1,2,3,

            2,3,4,	//right
            3,4,5,

            4,5,6,	//back
            5,6,7,

            0,1,6,	//left
            1,6,7,

            1,3,7,	//top
            3,5,7,

            0,2,6,	//bottom
            2,4,6
    };



    constexpr std::array<unsigned int, 36> createChessPieceIndices()
    {
        return {
                0, 1, 2,  2, 3, 0,      // Front face
                4, 5, 6,  6, 7, 4,      // Back face
                8, 9, 10, 10, 11, 8,    // Left face
                12, 13, 14, 14, 15, 12, // Right face
                16, 17, 18, 18, 19, 16, // Top face
                20, 21, 22, 22, 23, 20  // Bottom face
        };
    }

	constexpr std::array<unsigned int, 6 * 3 * 2> UnitCube3DTopologyTriangles = {
		0, 1, 2,    1, 2, 3,    2, 3, 4,     3, 4, 5,   4, 5, 6,   5, 6, 7,
		6, 7, 0,    7, 0, 1,    1, 3, 7,     3, 7, 5,   6, 0, 2,   2, 6, 4
	};
	template<typename T,typename U>
	std::vector<float>	UnitGridGeometry2D(T X, U Y) {
		std::vector<float> vbo;
		vbo.resize((X + 1) * (Y + 1) * 2);

		for (unsigned int j = 0; j < Y + 1; ++j) {
			float y = j / static_cast<float>(Y) - 0.5f;
			for (unsigned int i = 0; i < X + 1; ++i) {
				float x = i / static_cast<float>(X) - 0.5f;
				vbo[(j * (X + 1) + i) * 2 + 0] = x;
				vbo[(j * (X + 1) + i) * 2 + 1] = y;
			}
		}
		return vbo;
	}


    template<typename T, typename U>
    std::vector<unsigned int> ChessIndices(T X, U Y) {
        std::vector<unsigned int> indices;
        indices.resize(X * Y * 6);
        int counter = 0;

        for (unsigned int j = 0; j < Y; ++j) {
            for (unsigned int i = 0; i < X; ++i) {
                unsigned int baseIndex = j * (X + 1) + i;

                // Define indices for the two triangles forming a square
                indices[counter++] = baseIndex;
                indices[counter++] = baseIndex + X + 1;
                indices[counter++] = baseIndex + X + 2;

                indices[counter++] = baseIndex;
                indices[counter++] = baseIndex + 1;
                indices[counter++] = baseIndex + X + 2;
            }
        }
        return indices;
    }

    template<typename T>
    std::vector<float> Cube3DWNormals(T X) {
        std::vector<float>cube;
        cube.resize(3 * 24 * 2);
        for (int i = 0; i < cube.size(); i++)
        {
            cube[i] = UnitCube3D24WNormals[i] / X;
        }
        return cube;
    };



}

#endif // !__GEOMETRICTOOLS_H
