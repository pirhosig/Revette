#version 330 core
layout (location = 0) in vec2 vertexPos;

uniform mat4 transform;


void main()
{
	gl_Position = transform * vec4(0.0, vertexPos, 1.0);
}