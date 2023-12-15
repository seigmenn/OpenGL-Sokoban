//
// Created by auror on 15/12/2023.
//

#ifndef EXAMAUTUMN2023_SHADERS_H
#define EXAMAUTUMN2023_SHADERS_H

#include "string"

const std::string ChessBoardVertexShader = R"(
			#version 460 core

			layout (location = 0) in vec2 position;
			out vec4 vs_tcoords;
            out vec2 positions;
            uniform mat4 u_viewProjMat;
            uniform mat4 u_modMat;


			void main()
            {
			    gl_Position = u_viewProjMat * u_modMat * vec4(position.x, 0.0f, position.y, 1.0f);
	            vs_tcoords = u_modMat * vec4(position,0.0f,1.0f);
			    positions = position;
            }
			)";

const std::string ChessBoardFragmentShader = R"(
			#version 460 core

            #define M_PI 3.141592653589

            in vec4 vs_tcoords;
            in vec2 positions;
			out vec4 fragColor;

            uniform vec3 u_color1 = vec3(0.0);
            uniform vec3 u_color2 = vec3(1.0);
            uniform vec2 u_selectorPos = vec2(0.0);
            uniform vec3 u_selectorCol = vec3(0.0f,1.0f,0.0f);
            uniform vec2 u_divisions = vec2(0.0);
            uniform int u_texture = 0;

            layout(binding=0) uniform sampler2D u_floorTextureSampler;

			void main()
            {
              if(sin(M_PI*u_divisions.y*vs_tcoords.y) > 0){
                    if(sin(M_PI*u_divisions.x*vs_tcoords.x)> 0){
                        fragColor = vec4(u_color1,1.0);
                    }else{
                        fragColor = vec4(u_color2,0.0);
                    }
               }else{
                    if(sin(M_PI*u_divisions.y*vs_tcoords.x)>0){
                        fragColor = vec4(u_color2,0.0);
                    }else{
                         fragColor = vec4(u_color1,1.0);
                     }
                }
                vec2 selector_centered = u_selectorPos - vec2(0.5);

                if(vs_tcoords.x < (u_selectorPos.x+1)/u_divisions.x - 0.5 && vs_tcoords.x > u_selectorPos.x/u_divisions.x -0.5 &&
                   vs_tcoords.y < (u_selectorPos.y+1)/u_divisions.y - 0.5 && vs_tcoords.y > u_selectorPos.y/u_divisions.y -0.5 ){
                   fragColor = vec4(u_selectorCol,1.0);
                }

                if(u_texture == 1)
                {
                    fragColor = mix(fragColor,texture(u_floorTextureSampler, positions),0.5);
                }
			}
			)";

std::string ChessPieceFragmentShader = R"(
    #version 460 core

    in vec4 vs_pos;
    in vec3 vs_texPos;

    out vec4 finalColor;
    out vec4 fragPos;

    uniform vec4 u_cubeColor;
    uniform int u_texture = 0;

    layout(binding = 1) uniform samplerCube u_pieceTextureSampler;

    void main()
    {
        finalColor = u_cubeColor;
        if(u_texture == 1)
        {
            finalColor = mix(u_cubeColor,texture(u_pieceTextureSampler, vs_texPos),0.5);
        }

        fragPos = vs_pos;
    }

)";

std::string ChessPieceVertexShader = R"(
    #version 460 core

    layout (location = 0) in vec3 position;

    out vec4 vs_pos;
    out vec3 vs_texPos;

    uniform mat4 u_pieceViewProjMat;
    uniform mat4 u_pieceModMat;

    void main(){
        gl_Position = u_pieceViewProjMat * u_pieceModMat * vec4(position,1.0f);
        vs_pos = u_pieceModMat * vec4(position,1.0f);
        vs_texPos = position;
    }
)";

#endif //EXAMAUTUMN2023_SHADERS_H
