#version 460 core
in vec3 textureCoords;

out vec4 FragColor;

uniform sampler2DArray characterArray;

void main()
{
	vec4 textureColour = texture(characterArray, textureCoords);
	FragColor = textureColour;
	if (textureColour.a < 0.1)
		discard;
}