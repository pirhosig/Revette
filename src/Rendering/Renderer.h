#pragma once
#include <unordered_map>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "ShaderProgram.h"
#include "TileTexture.h"
#include "Mesh/MeshChunk.h"
#include "Mesh/MeshText.h"
#include "../Threading/ThreadPointerQueue.h"
#include "../World/ChunkPos.h"
#include "../World/ChunkPosHash.h"
#include "../World/Entities/EntityPosition.h"




class Renderer
{
public:
	Renderer(
		GLFWwindow* window,
		std::shared_ptr<ThreadPointerQueue<MeshDataChunk>> chunkMeshQueue,
		std::shared_ptr<ThreadQueue<ChunkPos>> chunkMeshQueueDeletion
	);

	void render(const EntityPosition& playerPos);
	void unqueueMeshes();
	void unloadMeshes(const ChunkPos& playerChunk);

private:
	// Mesh queue and storage
	std::shared_ptr<ThreadPointerQueue<MeshDataChunk>> threadQueueMeshes;
	std::shared_ptr<ThreadQueue<ChunkPos>> threadQueueMeshDeletion;
	std::unordered_map<ChunkPos, std::unique_ptr<MeshChunk>> meshesChunk;
	MeshText meshGUI;

	TileTexture tileTextureAtlas;
	TileTexture textureAtlasCharacters;

	ShaderProgram chunkShaderOpaque;
	ShaderProgram chunkShaderTransparent;
	ShaderProgram textShader;

	GLFWwindow* mainWindow;
};