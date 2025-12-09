#pragma once
#include "Window.h"



class Application
{
private:
	Window window;

public:
	Application() = default;

	Application(Application&&) = delete;
	Application(const Application&) = delete;
	Application operator=(Application&&) = delete;
	Application operator=(const Application&) = delete;

	void run();
};
