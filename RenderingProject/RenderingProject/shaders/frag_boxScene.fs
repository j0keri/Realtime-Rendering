#version 330 core

in vec2 texCoord;

out vec4 fragColor;

uniform sampler2D tex0;
uniform sampler2D tex1;
uniform float mixWeight;

void main()
{
	fragColor = mix(texture(tex0, texCoord), texture(tex1, texCoord), mixWeight);
}