#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../EntityPosition.h"
#include "ShaderProgram.h"
#include "TileTexture.h"



class Renderer
{
public:
	Renderer(GLFWwindow* window);

	void render(const EntityPosition& playerPos);

private:
	TileTexture tileTextureAtlas;

	ShaderProgram testShader;
	GLuint VAO;
	GLuint VBO;

	GLFWwindow* mainWindow;
};