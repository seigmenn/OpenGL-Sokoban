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
            uniform vec2 u_divisions = vec2(0.0);
            uniform int u_texture = 0;
            uniform vec3 u_lightPos;
            uniform vec3 u_lightColor;
            uniform vec3 u_viewPos;
            uniform float ambientStrength;
            uniform float specularStrength;
            uniform float diffuseStrength;


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

                if(u_texture == 1)
                {
                    fragColor = mix(fragColor,texture(u_floorTextureSampler, positions),0.5);
                }

    vec3 ambient = ambientStrength * u_lightColor;

    vec3 norm = normalize(vec3(0.0, 1.0, 0.0));
    vec3 lightDir = normalize(u_lightPos - vec3(vs_tcoords.x, 0.0, vs_tcoords.y));
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = u_lightColor * (diff * fragColor.rgb)*diffuseStrength;

    float specularStrength = 0.2;
    vec3 viewDir = normalize(u_viewPos - vec3(vs_tcoords.x, 0.0, vs_tcoords.y));
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = specularStrength * spec * u_lightColor;

    vec3 result = (ambient + diffuse + specular) * fragColor.rgb;

    fragColor = vec4(result, fragColor.a);
			}
			)";



std::string ChessPieceVertexShader = R"(
#version 460 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normals;

out VS_OUT {
    vec3 fragNormal;
    vec3 fragPosition;
    vec3 viewDir;
    vec3 vs_texture;
} vs_out;

uniform mat4 u_pieceViewProjMat;
uniform mat4 u_pieceModMat;

void main()
{
    vs_out.vs_texture = position;
    gl_Position = u_pieceViewProjMat * u_pieceModMat * vec4(position, 1.0);
    vs_out.fragNormal = mat3(transpose(inverse(u_pieceModMat))) * normals;
    vs_out.fragPosition = vec3(u_pieceModMat * vec4(position, 1.0));
    vs_out.viewDir = vec3(0.0, 0.0, 1.0);
}

)";

std::string ChessPieceFragmentShader = R"(
#version 460 core

in VS_OUT {
    vec3 fragNormal;
    vec3 fragPosition;
    vec3 viewDir;
    vec3 vs_texture;
} fs_in;



out vec4 FragColor;

uniform vec4 u_cubeColor;
uniform vec3 u_lightPos;
uniform vec3 u_lightColor;
uniform vec3 u_viewPos;
uniform float ambientStrength;
uniform float specularStrength;
uniform float diffuseStrength;
uniform int u_texture = 0;

layout(binding = 1) uniform samplerCube u_pieceTextureSampler;

void main()
{
    vec3 ambient = ambientStrength * u_lightColor;

    vec3 norm = normalize(fs_in.fragNormal);
    vec3 lightDir = normalize(u_lightPos - fs_in.fragPosition);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = u_lightColor * (diff * u_cubeColor.rgb)*diffuseStrength;

    float specularStrength = 0.5; // Adjust as needed
    vec3 viewDir = normalize(u_viewPos - fs_in.fragPosition);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = specularStrength * spec * u_lightColor;

    vec3 result = (ambient + diffuse + specular) * u_cubeColor.rgb;

    FragColor = vec4(result, u_cubeColor.a);

    if(u_texture == 1)
        {
            vec4 texColor = texture(u_pieceTextureSampler, fs_in.vs_texture);
        result *= texColor.rgb; // Multiply the lighting with the texture color
        FragColor = vec4(result, u_cubeColor.a);
        }
}

)";

#endif //EXAMAUTUMN2023_SHADERS_H
