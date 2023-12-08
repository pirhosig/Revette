#version 460 core
in vec3 TextureCoords;
in float Light;

out vec4 FragColor;
layout (depth_greater) out float gl_FragDepth;


uniform sampler2DArray tileAtlas;

void main()
{
	vec4 textureColour = texture(tileAtlas, TextureCoords);
	if (textureColour.a < 0.1)
		discard;
	FragColor = vec4(textureColour.xyz * Light, 1.0);
}