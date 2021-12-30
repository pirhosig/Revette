#include "RenderingLoop.h"

#include <chrono>
#include <stdexcept>
#include <thread>
#include <iostream>


RenderingLoop::RenderingLoop()
{
	// Initialize glfw and configure it
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Get window information
	GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* videoMode = glfwGetVideoMode(primaryMonitor);

	mainWindow = glfwCreateWindow(videoMode->width, videoMode->height, "Revette-3D", primaryMonitor, NULL);
	if (mainWindow == NULL) throw std::runtime_error("Unable to create window");

	glfwMakeContextCurrent(mainWindow);


	// Load opengl function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) throw std::runtime_error("Failed to load openGL function pointers");

	// Sets the user pointer for use in callback functions
	glfwSetWindowUserPointer(mainWindow, this);

	glViewport(0, 0, videoMode->width, videoMode->height);

	testShader.loadProgram("shader/testShader.vs", "shader/testShader.fs");

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 8, (void*)(0));
	glEnableVertexAttribArray(0);

	const float TEST_VERTICIES[] = {
		-0.5f, -0.5f,
		 0.5f, -0.5f,
		 0.0f,  0.5f
	};

	glBufferData(GL_ARRAY_BUFFER, 24, &TEST_VERTICIES[0], GL_STATIC_DRAW);
}



RenderingLoop::~RenderingLoop()
{
	glfwTerminate();
}



void RenderingLoop::runLoop(std::atomic<bool>& gameShouldClose)
{
	while (!gameShouldClose)
	{
		const auto frameEnd = std::chrono::steady_clock::now() + std::chrono::milliseconds(15);

		glfwPollEvents();
		if (glfwGetKey(mainWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		{
			gameShouldClose.store(true);
			break;
		}

		render();

		// Limit framerate
		std::this_thread::sleep_until(frameEnd);
	}
}



void RenderingLoop::render()
{
	glClearColor(0.53f, 0.52f, 0.83f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	testShader.use();
	glDrawArrays(GL_TRIANGLES, 0, 3);

	glfwSwapBuffers(mainWindow);
}


