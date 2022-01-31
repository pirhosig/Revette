#version 330 core
in vec3 TextureCoords;

out vec4 FragColor;

uniform sampler2DArray tileAtlas;

void main()
{
	vec4 textureColour = texture(tileAtlas, TextureCoords);
	if (textureColour.a < 0.1)
		discard;
	FragColor = textureColour;
}