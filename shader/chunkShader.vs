#version 330 core
layout (location = 0) in vec3 vertexPos;
layout (location = 1) in float vertexTextureIndex;
layout (location = 2) in vec2 vertexTextureCoords;

out vec3 TextureCoords;

uniform mat4 transform;

void main()
{
	gl_Position = transform * vec4(vertexPos, 1.0);
	TextureCoords = vec3(vertexTextureCoords / 255.0f, vertexTextureIndex);
}