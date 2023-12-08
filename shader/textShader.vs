#version 460 core
layout (location = 0) in vec2 characterPosition;
layout (location = 1) in vec3 vertexTextureCoords;

out vec3 textureCoords;

uniform vec2 characterSize;

void main()
{
	vec2 scaledPosition = characterSize * characterPosition;
	vec2 position = vec2(-1.0 + scaledPosition.x, 1.0 - scaledPosition.y);
	gl_Position = vec4(position, 0.0f, 1.0f);
	textureCoords = vertexTextureCoords;
}