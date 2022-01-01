#pragma once
#include <atomic>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "LoopTimer.h"
#include "Rendering/ShaderProgram.h"
#include "EntityPosition.h"



class RenderingLoop
{
public:
	RenderingLoop();
	~RenderingLoop();

	void runLoop(std::atomic<bool>& gameShouldClose);
private:
	void render();

	LoopTimer framerateCounter;

	EntityPosition playerPos;

	ShaderProgram testShader;
	GLuint VAO;
	GLuint VBO;

	GLFWwindow* mainWindow;
};