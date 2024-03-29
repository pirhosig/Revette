#include "Renderer.h"
#include <algorithm>
#include <stdio.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../GlobalLog.h"



inline bool withinLoadDistance(ChunkPos _pos, ChunkPos _centre)
{
	auto _offset = _pos.offset(_centre);
	return ((_offset.x * _offset.x + _offset.z * _offset.z <= LOAD_DISTANCE * LOAD_DISTANCE) &&
		(std::abs(_offset.y) <= LOAD_DISTANCE_VERTICAL));
}



Renderer::Renderer(
	GLFWwindow* window,
	std::shared_ptr<ThreadPointerQueue<MeshDataChunk>> chunkMeshQueue,
	std::shared_ptr<ThreadQueue<ChunkPos>> chunkMeshQueueDeletion
) :
	threadQueueMeshes{ chunkMeshQueue },
	threadQueueMeshDeletion{ chunkMeshQueueDeletion },
	tileTextureAtlas("res/texture_atlas.png", 16, 16, true),
	textureAtlasCharacters("res/character_set.png", 6, 8, false),
	chunkShaderOpaque("shader/chunkShader.vs", "shader/chunkShader.fs"),
	chunkShaderTransparent("shader/chunkShader.vs", "shader/chunkShaderTransparent.fs"),
	textShader("shader/textShader.vs", "shader/textShader.fs"),
	mainWindow{ window }
{
	GlobalLog.Write("Created renderer");
}



struct CmpChunkPos
{
	ChunkPos centre;

	bool operator()(const ChunkPos& a, const ChunkPos& b) const
	{
		return centre.distance(a) < centre.distance(b);
	}
};



void Renderer::render(const EntityPosition& playerPos)
{
	// Draw chunks
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glClearColor(0.52f, 0.70f, 0.89f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	{
		double rotationY = glm::radians(std::clamp(playerPos.yRotation, -89.9, 89.9));
		double rotationX = glm::radians(playerPos.xRotation);

		const glm::mat4 projection = glm::perspective(glm::radians(45.0), 1920.0 / 1080.0, 0.25, 1024.0);
		ChunkPos _playerChunk(playerPos);
		EntityPosition _playerLocalPos({
			playerPos.pos.X - _playerChunk.x * CHUNK_SIZE,
			playerPos.pos.Y - _playerChunk.y * CHUNK_SIZE,
			playerPos.pos.Z - _playerChunk.z * CHUNK_SIZE
		});
		const glm::vec3 pos = glm::vec3(_playerLocalPos.pos.X, _playerLocalPos.pos.Y + 3.0, _playerLocalPos.pos.Z) * 0.5f;
		const glm::vec3 front = glm::normalize(glm::vec3(
			cos(rotationX) * cos(rotationY),
			sin(rotationY),
			sin(rotationX) * cos(rotationY)
		));
		const glm::mat4 view = glm::lookAt(pos, pos + front, glm::vec3(0.0, 1.0, 0.0));
		const glm::mat4 projectionView = projection * view;

		chunkShaderOpaque.use();
		tileTextureAtlas.bindTexture();
		chunkShaderOpaque.setInt("tileAtlas", 0);
		for (auto& [_pos, _mesh] : meshesChunk)
			_mesh->drawOpaque(chunkShaderOpaque, projectionView, _playerChunk);

		// Draw transparent objects
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		chunkShaderTransparent.use();
		chunkShaderTransparent.setInt("tileAtlas", 0);
		for (auto& [_pos, _mesh] : meshesChunk)
			_mesh->drawTransparent(chunkShaderTransparent, projectionView, _playerChunk);
	}

	// Draw GUI
	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	textShader.use();
	textureAtlasCharacters.bindTexture();
	textShader.setInt("characterArray", 0);
	textShader.setVec2("characterSize", glm::vec2(24.0 / 1920.0, 32.0 / 1080.0));

	// Update coordinates
	{
		char coordinateString[33]{};
		int _length = snprintf(
			&coordinateString[0],
			32,
			"%8.2lf %8.2lf %8.2lf",
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
		ChunkPos _pos = queue.front()->position;
		meshesChunk.insert({ _pos, std::make_unique<MeshChunk>(std::move(queue.front())) });
		queue.pop();
	}
}



void Renderer::unloadMeshes(const ChunkPos& playerChunk)
{
	std::queue<ChunkPos> removeQueue;

	auto it = meshesChunk.begin();
	while (it != meshesChunk.end())
	{
		if (!withinLoadDistance(it->first, playerChunk))
		{
			removeQueue.push(it->first);
			it = meshesChunk.erase(it);
		}
		else it++;
	}

	// Add the removed chunks if any were removed
	if (removeQueue.size()) threadQueueMeshDeletion->mergeQueue(removeQueue);
}
