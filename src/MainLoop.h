#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "LoopTimer.h"
#include "Rendering/Renderer.h"
#include "World/World.h"



class MainLoop
{
public:
	MainLoop();
	MainLoop(const MainLoop&) = delete;
	~MainLoop();

	void run();
private:
	GLFWwindow* window;

	int windowWidth;
	int windowHeight;
};
