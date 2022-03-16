#version 330 core
in vec3 TextureCoords;
in float Light;

out vec4 FragColor;

uniform sampler2DArray tileAtlas;

void main()
{
	vec4 textureColour = texture(tileAtlas, TextureCoords);
	if (textureColour.a < 0.1)
		discard;
	vec4 colourAdjusted = vec4(textureColour.xyz * Light, textureColour.w);
	FragColor = colourAdjusted;
}