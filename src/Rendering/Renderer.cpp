#include "Renderer.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>



Renderer::Renderer(GLFWwindow* window, std::shared_ptr<ThreadPointerQueue<MeshDataChunk>> chunkMeshQueue) :
	chunkShader("shader/chunkShader.vs", "shader/chunkShader.fs"),
	tileTextureAtlas("res/texture_atlas.png"),
	threadQueueMeshes(chunkMeshQueue),
	mainWindow(window)
{}



void Renderer::render(const EntityPosition& playerPos)
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glClearColor(0.53f, 0.52f, 0.83f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	double rotationY = glm::radians(fmax(fmin(playerPos.yRotation, 89.5), -89.5));
	double rotationX = glm::radians(playerPos.xRotation);

	const glm::mat4 projection = glm::perspective(glm::radians(45.0), 1920.0 / 1080.0, 0.1, 1000.0);
	const glm::vec3 pos(playerPos.X, playerPos.Y, playerPos.Z);
	const glm::vec3 front = glm::normalize(glm::vec3(
		cos(rotationX) * cos(rotationY),
		sin(rotationY),
		sin(rotationX) * cos(rotationY)
	));
	const glm::mat4 view = glm::lookAt(pos, pos + front, glm::vec3(0.0, 1.0, 0.0));
	const glm::mat4 projectionView = projection * view;

	chunkShader.use();
	tileTextureAtlas.bindTexture();
	chunkShader.setInt("tileAtlas", 0);
	for (const auto& mesh : meshesChunk)
	{
		mesh->draw(chunkShader, projectionView);
	}

	glfwSwapBuffers(mainWindow);
}



void Renderer::unqueueMeshes()
{
	std::queue<std::unique_ptr<MeshDataChunk>> queue;
	threadQueueMeshes->getQueue(queue);
	while (!queue.empty())
	{
		meshesChunk.insert(std::make_unique<MeshChunk>(std::move(queue.front())));
		queue.pop();
	}
}