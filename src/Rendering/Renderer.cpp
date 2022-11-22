#include "Renderer.h"
#include <stdio.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>



inline bool withinRenderingDistance(ChunkPos _pos, ChunkPos _centre)
{
	auto _offset = _pos.offset(_centre);
	return (std::abs(_offset.x) < LOAD_DISTANCE - 1 &&
		std::abs(_offset.y) < LOAD_DISTANCE_VERTICAL - 1 &&
		std::abs(_offset.z) < LOAD_DISTANCE - 1);
}



Renderer::Renderer(
	GLFWwindow* window,
	std::shared_ptr<ThreadPointerQueue<MeshDataChunk>> chunkMeshQueue,
	std::shared_ptr<ThreadQueue<ChunkPos>> chunkMeshQueueDeletion
) :
	chunkShader("shader/chunkShader.vs", "shader/chunkShader.fs"),
	textShader("shader/textShader.vs", "shader/textShader.fs"),
	tileTextureAtlas("res/texture_atlas.png", 16, 16, true),
	textureAtlasCharacters("res/character_set.png", 6, 8, false),
	threadQueueMeshDeletion{ chunkMeshQueueDeletion },
	threadQueueMeshes{ chunkMeshQueue },
	mainWindow{ window }
{}



void Renderer::render(const EntityPosition& playerPos)
{
	// Draw chunks
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glClearColor(0.52f, 0.70f, 0.89f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	{
		double rotationY = glm::radians(fmax(fmin(playerPos.yRotation, 89.9), -89.9));
		double rotationX = glm::radians(playerPos.xRotation);

		constexpr double FOV_Y = 45.0;
		constexpr double ASPECT = 1920.0 / 1080.0;

		const glm::mat4 projection = glm::perspective(glm::radians(FOV_Y), ASPECT, 0.1, 2048.0);
		ChunkPos _playerChunk(playerPos);
		EntityPosition _playerLocalPos({
			playerPos.pos.X - _playerChunk.x * CHUNK_SIZE,
			playerPos.pos.Y - _playerChunk.y * CHUNK_SIZE,
			playerPos.pos.Z - _playerChunk.z * CHUNK_SIZE
		});
		const glm::vec3 pos = glm::vec3(_playerLocalPos.pos.X, _playerLocalPos.pos.Y + 2.5, _playerLocalPos.pos.Z) * 0.25f;
		const glm::vec3 front = glm::normalize(glm::vec3(
			cos(rotationX) * cos(rotationY),
			sin(rotationY),
			sin(rotationX) * cos(rotationY)
		));
		const glm::mat4 view = glm::lookAt(pos, pos + front, glm::vec3(0.0, 1.0, 0.0));
		const glm::mat4 projectionView = glm::scale(projection * view, glm::vec3(0.25f));

		chunkShader.use();
		tileTextureAtlas.bindTexture();
		chunkShader.setInt("tileAtlas", 0);
		for (const auto& mesh : meshesChunk) {
			mesh->draw(chunkShader, projectionView, _playerChunk);
		}
	}

	// Draw GUI
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	textShader.use();
	textureAtlasCharacters.bindTexture();
	textShader.setInt("characterArray", 0);
	textShader.setVec2("characterSize", glm::vec2(24.0 / 1920.0, 32.0 / 1080.0));

	// Update coordinates
	{
		char coordinateString[32]{};
		int _length = sprintf_s(
			&coordinateString[0],
			32,
			"%8.1lf %8.1lf %8.1lf",
			playerPos.pos.X,
			playerPos.pos.Y,
			playerPos.pos.Z
		);

		meshGUI.update(&coordinateString[0], _length);
	}

	meshGUI.draw();

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



void Renderer::unloadMeshes(const ChunkPos& playerChunk)
{
	std::queue<ChunkPos> removeQueue;

	std::unordered_set<std::unique_ptr<MeshChunk>>::iterator it = meshesChunk.begin();
	while (it != meshesChunk.end())
	{
		auto _pos = (*it)->getPosition();
		if (!withinRenderingDistance(_pos, playerChunk))
		{
			it = meshesChunk.erase(it);
			removeQueue.push(_pos);
		}
		else it++;
	}

	// Add the removed chunks if any were removed
	if (removeQueue.size()) threadQueueMeshDeletion->mergeQueue(removeQueue);
}
