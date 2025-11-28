#pragma once
#include "Window.h"



class MainLoop
{
private:
	Window window;

public:
	MainLoop() = default;

	MainLoop(MainLoop&&) = delete;
	MainLoop(const MainLoop&) = delete;
	MainLoop operator=(MainLoop&&) = delete;
	MainLoop operator=(const MainLoop&) = delete;

	void run();
};
