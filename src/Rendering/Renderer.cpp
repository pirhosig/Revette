#include "Renderer.h"
#include <algorithm>

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
	chunkShaderOpaque("shader/chunkShader.vs", "shader/chunkShader.fs"),
	chunkShaderTransparent("shader/chunkShader.vs", "shader/chunkShaderTransparent.fs"),
	textShader("shader/textShader.vs", "shader/textShader.fs"),
	tileTextureAtlas("res/texture_atlas.png", 16, 16, true),
	textureAtlasCharacters("res/character_set.png", 6, 8, false),
	threadQueueMeshDeletion{ chunkMeshQueueDeletion },
	threadQueueMeshes{ chunkMeshQueue },
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
		double rotationY = glm::radians(fmax(fmin(playerPos.yRotation, 89.9), -89.9));
		double rotationX = glm::radians(playerPos.xRotation);

		constexpr double FOV_Y = 45.0;
		constexpr double ASPECT = 1920.0 / 1080.0;

		const glm::mat4 projection = glm::perspective(glm::radians(FOV_Y), ASPECT, 0.1, 1024.0);
		ChunkPos _playerChunk(playerPos);
		EntityPosition _playerLocalPos({
			playerPos.pos.X - _playerChunk.x * CHUNK_SIZE,
			playerPos.pos.Y - _playerChunk.y * CHUNK_SIZE,
			playerPos.pos.Z - _playerChunk.z * CHUNK_SIZE
		});
		const glm::vec3 pos = glm::vec3(_playerLocalPos.pos.X, _playerLocalPos.pos.Y + 2.5, _playerLocalPos.pos.Z) * 0.5f;
		const glm::vec3 front = glm::normalize(glm::vec3(
			cos(rotationX) * cos(rotationY),
			sin(rotationY),
			sin(rotationX) * cos(rotationY)
		));
		const glm::mat4 view = glm::lookAt(pos, pos + front, glm::vec3(0.0, 1.0, 0.0));
		const glm::mat4 projectionView = projection * view;

		// Get sorted array of meshes
		std::vector<ChunkPos> _meshPositions;
		_meshPositions.reserve(meshesChunk.size());
		for (auto& [_pos, _mesh] : meshesChunk) _meshPositions.push_back(_pos);
		std::sort(_meshPositions.begin(), _meshPositions.end(), CmpChunkPos{_playerChunk});

		chunkShaderOpaque.use();
		tileTextureAtlas.bindTexture();
		chunkShaderOpaque.setInt("tileAtlas", 0);
		for (const auto& _pos : _meshPositions) meshesChunk[_pos]->drawOpaque(chunkShaderOpaque, projectionView, _playerChunk);

		// Draw transparent objects
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		chunkShaderTransparent.use();
		chunkShaderTransparent.setInt("tileAtlas", 0);
		for (auto it = _meshPositions.rbegin(); it != _meshPositions.rend(); ++it)
			meshesChunk[*it]->drawTransparent(chunkShaderTransparent, projectionView, _playerChunk);
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
		meshesChunk.insert({ queue.front()->position, std::make_unique<MeshChunk>(std::move(queue.front())) });
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
			it = meshesChunk.erase(it);
			removeQueue.push(it->first);
		}
		else it++;
	}

	// Add the removed chunks if any were removed
	if (removeQueue.size()) threadQueueMeshDeletion->mergeQueue(removeQueue);
}
