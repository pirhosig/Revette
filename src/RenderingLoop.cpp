#include "RenderingLoop.h"

#include <chrono>
#include <stdexcept>
#include <thread>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

constexpr double PLAYER_SPEED = 3.0;



RenderingLoop::RenderingLoop() : framerateCounter(1024)
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
		 0.0f,  0.0f,
		 0.5f,  1.5f,
		 0.0f,  1.0f
	};

	glBufferData(GL_ARRAY_BUFFER, 24, &TEST_VERTICIES[0], GL_STATIC_DRAW);
}



RenderingLoop::~RenderingLoop()
{
	glfwTerminate();
}



void RenderingLoop::runLoop(std::atomic<bool>& gameShouldClose)
{
	auto lastFrame = std::chrono::steady_clock::now();

	while (!gameShouldClose)
	{
		const auto frameBegin = std::chrono::steady_clock::now();
		const auto frameEnd = frameBegin + std::chrono::milliseconds(15);
		const std::chrono::duration<double> timeElapsed = frameBegin - lastFrame;
		const double deltaTime = timeElapsed.count();
		lastFrame = frameBegin;

		glfwPollEvents();
		if (glfwGetKey(mainWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		{
			gameShouldClose.store(true);
			break;
		}
		if (glfwGetKey(mainWindow, GLFW_KEY_W) == GLFW_PRESS) playerPos.moveForward(deltaTime * PLAYER_SPEED);
		if (glfwGetKey(mainWindow, GLFW_KEY_S) == GLFW_PRESS) playerPos.moveForward(deltaTime * -PLAYER_SPEED);
		if (glfwGetKey(mainWindow, GLFW_KEY_A) == GLFW_PRESS) playerPos.moveSideways(deltaTime * PLAYER_SPEED);
		if (glfwGetKey(mainWindow, GLFW_KEY_D) == GLFW_PRESS) playerPos.moveSideways(deltaTime * -PLAYER_SPEED);
		if (glfwGetKey(mainWindow, GLFW_KEY_SPACE) == GLFW_PRESS) playerPos.moveVertical(deltaTime * PLAYER_SPEED);
		if (glfwGetKey(mainWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) playerPos.moveVertical(deltaTime * -PLAYER_SPEED);

		render();

		// Limit framerate
		std::this_thread::sleep_until(frameEnd);
		// Calculate framerate
		const auto frameEndTime = std::chrono::steady_clock::now();
		const auto frameTime = std::chrono::duration_cast<std::chrono::microseconds>(frameEndTime - frameBegin);
		framerateCounter.addTime(frameTime.count());
	}
}



void RenderingLoop::render()
{
	glClearColor(0.53f, 0.52f, 0.83f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	const glm::mat4 projection = glm::perspective(glm::radians(45.0), 1920.0 / 1080.0, 0.1, 100.0);
	const glm::vec3 pos(playerPos.X, playerPos.Y, playerPos.Z);
	const glm::vec front = glm::normalize(glm::vec3(
		cos(glm::radians(playerPos.xRotation)) * cos(glm::radians(playerPos.yRotation)),
		sin(glm::radians(playerPos.yRotation)),
		sin(glm::radians(playerPos.xRotation)) * cos(glm::radians(playerPos.yRotation))
	));
	const glm::mat4 view = glm::lookAt(pos, pos + front, glm::vec3(0.0, 1.0, 0.0));
	const glm::mat4 projectionView = projection * view;

	testShader.use();
	testShader.setMat4("transform", projectionView);

	glDrawArrays(GL_TRIANGLES, 0, 3);

	glfwSwapBuffers(mainWindow);
}


