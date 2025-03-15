#version 330 core

out vec4 fragColor;

uniform vec3 lightColor;


void main()
{
    fragColor = vec4(lightColor * 2, 1.0);
}