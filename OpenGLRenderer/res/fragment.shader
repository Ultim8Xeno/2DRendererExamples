#version 460 core

out vec4 FragColor;

in vec4 outColor;
in vec2 outTexCoords;
in flat int outTexID;

uniform sampler2D u_Textures[16];

void main()
{
    FragColor = texture(u_Textures[int(outTexID)], outTexCoords).rgba * outColor;
}