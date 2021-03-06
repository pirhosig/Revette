#pragma once
#include <memory>
#include <unordered_map>
#include <glm/glm.hpp>
#include "Constants.h"
#include "Chunk.h"
#include "Rendering/TextureArray.h"
#include "World Gen/WorldInterface.h"
#include "World Gen/WorldGenerator.h"

typedef std::unique_ptr<Chunk>& chunkReference;



class TileMap : public WorldInterface
{
public:
	TileMap();

	bool collisionQuery(double x, double y, double x2, double y2);
	std::unique_ptr<Chunk>& getChunk(unsigned x, unsigned y);
	virtual Tile getTile(unsigned tileX, unsigned tileY) final;
	virtual void setTile(unsigned int tileX, unsigned int tileY, Tile tileType) final;

	bool loadChunks();
	void populateChunks();
	bool drawChunks(std::unique_ptr<Shader>& shader, TextureArray& tilemap, const glm::mat4& projection, const glm::vec2& cameraOffset);

	std::shared_ptr<WorldGenerator> terrainGenerator;
private:
	std::unordered_map<unsigned int, std::unique_ptr<Chunk>> chunks;
};

