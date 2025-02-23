#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec3 ourColor;
out vec2 texCoord;

uniform bool flip;
uniform float offsetX;
uniform float offsetY;
uniform mat4 transform;

void main()
{
    vec4 newPos = transform * vec4(aPos, 1.0);
    
    if (flip)
    {
        gl_Position = vec4(offsetX, offsetY, 0.0, 0.0)
                      + vec4(newPos.x, -newPos.y, newPos.z, 1.0);
    }
    else
    {
        gl_Position = vec4(offsetX, offsetY, 0.0, 0.0)
                      + vec4(newPos.x, newPos.y, newPos.z, 1.0);
    }
    
    ourColor = aColor;
    texCoord = aTexCoord;
}