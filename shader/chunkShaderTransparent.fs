#version 460 core
in vec3 TextureCoords;
in float Light;

out vec4 FragColor;

uniform sampler2DArray tileAtlas;

void main()
{
	vec4 textureColour = texture(tileAtlas, TextureCoords);
	FragColor = vec4(textureColour.xyz * Light, textureColour.a);
}