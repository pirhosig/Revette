#include "Application.h"

#include <atomic>
#include <barrier>
#include <thread>

#include "LoopGame.h"
#include "GlobalLog.h"
#include "GlobalApplicationState.h"
#include "Settings.h"
#include "Rendering/Renderer.h"
#include "World/Generation/WorldGenerator.h"



namespace {

void runGameThread(
	std::barrier<>& loadSyncPoint,
	GlobalApplicationState& globalApplicationState,
	const Settings& settings,
	GLFWwindow* window
) try {
	LoopGame loop(globalApplicationState, settings, window);
	loadSyncPoint.arrive_and_wait();
	loop.run();
}
catch (const std::exception& error) {
	GlobalLog.Write(std::string("Game thread exception: ") + error.what());
}



void runRenderThread(
	std::barrier<>& loadSyncPoint,
	GlobalApplicationState& globalApplicationState,
	const Settings& settings,
	GLFWwindow* window
) try {
	Renderer renderer(globalApplicationState, settings, window);
	loadSyncPoint.arrive_and_wait();
	renderer.run();
}
catch (const std::exception& error) {
	GlobalLog.Write(std::string("Rendering thread exception: ") + error.what());
}



void runGenerationThread(
	std::barrier<>& loadSyncPoint,
	GlobalApplicationState& globalApplicationState,
	const Settings& settings
) try {
	WorldGenerator worldGenerator(
		globalApplicationState,
		settings
	);
	loadSyncPoint.arrive_and_wait();
	worldGenerator.run();
}
catch (const std::exception& error) {
	GlobalLog.Write(std::string("Generation thread exception: ") + error.what());
}

}



void Application::run() {
	std::barrier loadSyncPoint(3);

	Settings settings;
	
	GlobalApplicationState globalApplicationState;

	std::jthread renderThread(
		runRenderThread,
		std::ref(loadSyncPoint),
		std::ref(globalApplicationState),
		std::cref(settings),
		window.get()
	);

	std::jthread gameThread(
		runGameThread,
		std::ref(loadSyncPoint),
		std::ref(globalApplicationState),
		std::cref(settings),
		window.get()
	);

	std::jthread generationThread(
		runGenerationThread,
		std::ref(loadSyncPoint),
		std::ref(globalApplicationState),
		std::cref(settings)
	);
}

