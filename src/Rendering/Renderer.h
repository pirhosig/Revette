#pragma once
#include <unordered_set>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "ShaderProgram.h"
#include "TileTexture.h"
#include "Mesh/MeshChunk.h"
#include "../Threading/ThreadPointerQueue.h"
#include "../World/Entities/EntityPosition.h"



class Renderer
{
public:
	Renderer(GLFWwindow* window, std::shared_ptr<ThreadPointerQueue<MeshDataChunk>> chunkMeshQueue);

	void render(const EntityPosition& playerPos);
	void unqueueMeshes();

private:
	// Mesh queue and storage
	std::shared_ptr<ThreadPointerQueue<MeshDataChunk>> threadQueueMeshes;
	std::unordered_set<std::unique_ptr<MeshChunk>> meshesChunk;

	TileTexture tileTextureAtlas;
	ShaderProgram chunkShader;
	GLFWwindow* mainWindow;
};