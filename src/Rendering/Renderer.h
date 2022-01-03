#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "ShaderProgram.h"
#include "../EntityPosition.h"



class Renderer
{
public:
	Renderer(GLFWwindow* window);

	void render(const EntityPosition& playerPos);

private:

	ShaderProgram testShader;
	GLuint VAO;
	GLuint VBO;

	GLFWwindow* mainWindow;
};