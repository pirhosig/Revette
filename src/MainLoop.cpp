#include "MainLoop.h"
#include "LoopGame.h"

#include "GlobalLog.h"



MainLoop::MainLoop()
{
	// Initialize glfw and configure it
	if (glfwInit() == GL_FALSE) throw std::runtime_error("Failed to initialise GLFW");
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Get window information
	GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* videoMode = glfwGetVideoMode(primaryMonitor);
	windowWidth = videoMode->width;
	windowHeight = videoMode->height;

	// Create a window and verify that it was created, then set it as the current opengl context on this thread
	window = glfwCreateWindow(windowWidth, windowHeight, "Revette-3D", primaryMonitor, NULL);
	if (window == NULL) throw std::runtime_error("Failed to create window");
	glfwMakeContextCurrent(window);

	// Load opengl function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) throw std::runtime_error("Failed to load openGL function pointers");

	// Set the viewport to match the screen height and width
	glViewport(0, 0, windowWidth, windowHeight);

	GlobalLog.Write("Created opengl context");
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

