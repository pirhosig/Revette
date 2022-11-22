#include "MainLoop.h"
#include "LoopGame.h"



MainLoop::MainLoop()
{
	// Initialize glfw and configure it
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Get window information
	GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* videoMode = glfwGetVideoMode(primaryMonitor);
	windowWidth = videoMode->width;
	windowHeight = videoMode->height;

	// Create a window and verify that it was created, then set it as the current opengl context on this thread
	window = glfwCreateWindow(windowWidth, windowHeight, "Revette-3D", primaryMonitor, NULL);
	if (window == NULL) throw std::runtime_error("Unable to create window");
	glfwMakeContextCurrent(window);

	// Load opengl function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) throw std::runtime_error("Failed to load openGL function pointers");

	// Set the viewport to match the screen height and width
	glViewport(0, 0, windowWidth, windowHeight);
}



MainLoop::~MainLoop()
{
	glfwTerminate();
}



void MainLoop::run()
{
	std::shared_ptr<ThreadPointerQueue<MeshDataChunk>> chunkMeshQueue = std::make_shared<ThreadPointerQueue<MeshDataChunk>>();
	std::shared_ptr<ThreadQueue<ChunkPos>> chunkMeshQueueDeletion = std::make_shared<ThreadQueue<ChunkPos>>();
	LoopGame loop(window, chunkMeshQueue, chunkMeshQueueDeletion);
	loop.run();
}

