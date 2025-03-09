#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 fragPos;
out vec3 normalVecView;
out vec3 lightPosView;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatView;
uniform vec3 lightPos;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    fragPos = vec3(view * model * vec4(aPos, 1.0));
    normalVecView = normalMatView * aNormal;
    lightPosView = vec3(view * vec4(lightPos, 1.0));
} 