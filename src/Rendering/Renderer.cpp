#include "Renderer.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>



Renderer::Renderer(GLFWwindow* window, std::shared_ptr<ThreadQueueMeshes> chunkMeshQueue) : 
	chunkShader("shader/chunkShader.vs", "shader/chunkShader.fs"),
	tileTextureAtlas("res/texture_atlas.png"),
	threadQueueMeshes(chunkMeshQueue)
{
	mainWindow = window;
}



void Renderer::render(const EntityPosition& playerPos)
{
	glClearColor(0.53f, 0.52f, 0.83f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	const glm::mat4 projection = glm::perspective(glm::radians(45.0), 1920.0 / 1080.0, 0.1, 100.0);
	const glm::vec3 pos(playerPos.X, playerPos.Y, playerPos.Z);
	const glm::vec front = glm::normalize(glm::vec3(
		cos(glm::radians(playerPos.xRotation)) * cos(glm::radians(playerPos.yRotation)),
		sin(glm::radians(playerPos.yRotation)),
		sin(glm::radians(playerPos.xRotation)) * cos(glm::radians(playerPos.yRotation))
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