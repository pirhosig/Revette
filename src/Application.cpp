#include "Application.h"

#include <atomic>
#include <thread>

#include "LoopGame.h"
#include "GlobalLog.h"
#include "Rendering/Renderer.h"



void runGameThread(
	GLFWwindow* window,
	std::atomic_bool& applicationShouldTerminate,
	std::shared_ptr<SharedGameRendererState> sharedRendererState
) try {
	LoopGame loop(window, applicationShouldTerminate, std::move(sharedRendererState));
	loop.run();
}
catch (const std::exception& error) {
	GlobalLog.Write(std::string("Game thread exception: ") + error.what());
}



void runRenderThread(
	GLFWwindow* window,
	std::atomic_bool& applicationShouldTerminate,
	std::shared_ptr<SharedGameRendererState> sharedGameState
) try {
	Renderer renderer(window, applicationShouldTerminate, std::move(sharedGameState));
	renderer.run();
}
catch (const std::exception& error) {
	GlobalLog.Write(std::string("Rendering thread exception: ") + error.what());
}



void Application::run() {
	std::atomic_bool applicationShouldTerminate;
	auto sharedGameRendererState{std::make_shared<SharedGameRendererState>()};

	std::jthread renderThread(
		runRenderThread,
		window.get(),
		std::ref(applicationShouldTerminate),
		sharedGameRendererState
	);

	std::jthread gameThread(
		runGameThread,
		window.get(),
		std::ref(applicationShouldTerminate),
		sharedGameRendererState
	);
}

