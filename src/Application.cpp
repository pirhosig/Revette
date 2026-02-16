#include "Application.h"

#include <atomic>
#include <thread>

#include "LoopGame.h"
#include "GlobalLog.h"
#include "Settings.h"
#include "Rendering/Renderer.h"



void runGameThread(
	const Settings& settings,
	GLFWwindow* window,
	std::atomic_bool& applicationShouldTerminate,
	std::shared_ptr<SharedGameRendererState> sharedRendererState
) try {
	LoopGame loop(settings, window, applicationShouldTerminate, std::move(sharedRendererState));
	loop.run();
}
catch (const std::exception& error) {
	GlobalLog.Write(std::string("Game thread exception: ") + error.what());
}



void runRenderThread(
	const Settings& settings,
	GLFWwindow* window,
	std::atomic_bool& applicationShouldTerminate,
	std::shared_ptr<SharedGameRendererState> sharedGameState
) try {
	Renderer renderer(settings, window, applicationShouldTerminate, std::move(sharedGameState));
	renderer.run();
}
catch (const std::exception& error) {
	GlobalLog.Write(std::string("Rendering thread exception: ") + error.what());
}



void Application::run() {
	Settings settings;

	std::atomic_bool applicationShouldTerminate;
	auto sharedGameRendererState{std::make_shared<SharedGameRendererState>()};

	std::jthread renderThread(
		runRenderThread,
		std::cref(settings),
		window.get(),
		std::ref(applicationShouldTerminate),
		sharedGameRendererState
	);

	std::jthread gameThread(
		runGameThread,
		std::cref(settings),
		window.get(),
		std::ref(applicationShouldTerminate),
		sharedGameRendererState
	);
}

