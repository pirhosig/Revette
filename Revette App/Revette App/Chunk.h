#pragma once
#include <memory>

#include "Constants.h"
#include "ChunkMesh.h"
#include "Rendering/Shader.h"
#include "Tile.h"
class WorldGenerator;
#include "World Gen/WorldGenerator.h"



class Chunk
{
public:
	Chunk(unsigned x, unsigned y, std::shared_ptr<WorldGenerator>& generator);

	bool generateChunk();
	Tile getChunkTile(unsigned tileX, unsigned tileY);
	void setChunkTile(unsigned tileX, unsigned tileY, Tile tile);

	bool generateMesh();
	bool draw(std::unique_ptr<Shader>& shader, const glm::mat4& projection, const glm::vec2& cameraOffset);

	unsigned chunkX;
	unsigned chunkY;

	std::unique_ptr<Tile[]> tileData;


private:
	bool dataHasChanged = false;

	std::unique_ptr<ChunkMesh> tileMesh;

	std::shared_ptr<WorldGenerator> terrainGenerator;
};

