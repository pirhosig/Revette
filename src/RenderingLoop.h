#pragma once
#include <atomic>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Rendering/ShaderProgram.h"



class RenderingLoop
{
public:
	RenderingLoop();
	~RenderingLoop();

	void runLoop(std::atomic<bool>& gameShouldClose);
private:
	void render();

	ShaderProgram testShader;
	GLuint VAO;
	GLuint VBO;

	GLFWwindow* mainWindow;
};