#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "ShaderProgram.h"
#include "TileTexture.h"
#include "../EntityPosition.h"
#include "../Threading/ThreadQueueMeshes.h"



class Renderer
{
public:
	Renderer(GLFWwindow* window, std::shared_ptr<ThreadQueueMeshes> chunkMeshQueue);

	void render(const EntityPosition& playerPos);

private:
	std::shared_ptr<ThreadQueueMeshes> threadQueueMeshes;

	TileTexture tileTextureAtlas;

	ShaderProgram testShader;
	GLuint VAO;
	GLuint VBO;

	GLFWwindow* mainWindow;
};