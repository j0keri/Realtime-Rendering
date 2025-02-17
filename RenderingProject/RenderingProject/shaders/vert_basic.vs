#version 330 core

layout (location = 0) in vec3 aPos;

uniform bool flip;
uniform float offsetX;
uniform float offsetY;

void main()
{
    if (flip)
    {
        gl_Position = vec4(aPos.x + offsetX, -aPos.y + offsetY, aPos.z, 1.0);
    }
    else
    {
        gl_Position = vec4(aPos.x + offsetX, aPos.y + offsetY, aPos.z, 1.0);
    }
}