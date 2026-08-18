#pragma once
#include "Application/GlobalApplicationState.h"
#include "Rendering/Renderer.h"
#include "World/World.h"



class LoopGame {
private:
	GlobalApplicationState& globalApplicationState;

	double cursorLastX;
	double cursorLastY;

	World world;

	Entity player;

	class GLFWwindow* window;

private:
	void processInput(const double deltaTime);
	void cursorPositionCallback(double xpos, double ypos);

public:
	explicit LoopGame(
		GlobalApplicationState& _globalApplicationState,
		const Settings& settings,
		class GLFWwindow* _window
	);

	void run();

	static void cursorPositionCallbackWrapper(GLFWwindow* window, double xpos, double ypos);
};
