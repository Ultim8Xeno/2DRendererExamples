#version 460 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec4 color;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in int texID;

out vec4 outColor;
out vec2 outTexCoords;
out flat int outTexID;

void main()
{
	outColor = color;
    outTexCoords = texCoords;
    outTexID = texID;
	gl_Position = vec4(pos, 1.0);
}