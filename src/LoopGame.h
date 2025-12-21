#pragma once
#include "LoopTimer.h"
#include "Rendering/Renderer.h"
#include "World/World.h"
class GLFWwindow;



class LoopGame {
private:
	std::atomic_bool& applicationShouldTerminate;
	std::shared_ptr<SharedGameRendererState> sharedRendererState;

	double cursorLastX;
	double cursorLastY;

	World world;

	Entity player;

	GLFWwindow* window;

private:
	void processInput(const double deltaTime);
	void cursorPositionCallback(double xpos, double ypos);

public:
	LoopGame(
		GLFWwindow* _window,
		std::atomic_bool& _applicationShouldTerminate,
		std::shared_ptr<SharedGameRendererState> _sharedRendererState
	);

	void run();

	static void cursorPositionCallbackWrapper(GLFWwindow* window, double xpos, double ypos);
};
