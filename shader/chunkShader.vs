#version 330 core
layout (location = 0) in vec4 vertexPos;
layout (location = 1) in float vertexTextureIndex;
layout (location = 2) in vec2 vertexTextureCoords;
layout (location = 3) in float vertexLight;

out vec3 TextureCoords;
out float Light;

uniform mat4 transform;

void main()
{
	gl_Position = transform * vec4(vertexPos.xyz / 32.0f, 1.0f);
	TextureCoords = vec3(vertexTextureCoords, vertexTextureIndex);
	Light = vertexLight;
}