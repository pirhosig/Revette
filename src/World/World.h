#pragma once
#include <mutex>
#include <unordered_map>

#include "Block.h"
#include "BlockHash.h"
#include "Chunk.h"
#include "ChunkPos.h"
#include "ChunkStatusMap.h"
#include "Entities/Entity.h"
#include "Generation/GeneratorChunkParameters.h"
#include "Generation/GeneratorChunkNoise.h"
#include "Application/GlobalApplicationState.h"
#include "Settings.h"
#include "Rendering/Mesh/MeshChunk.h"
#include "Threading/SharedGameRendererState.h"



class World {
public:
	struct GlobalStateType {
		std::mutex mutexForIncomingChunks;
		std::vector<std::unique_ptr<Chunk>> incomingChunks;

		GlobalStateType();
	};
	static GlobalStateType GlobalState;

private:
	GlobalApplicationState& globalApplicationState;
	const Settings& settings;

	// Chunk storage
	std::unordered_map<long long, Entity> mapEntities;
	std::unordered_map<ChunkPos, std::unique_ptr<Chunk>> mapChunks;

	// Chunk loading information
	i64 chunkUnloadDistanceSquared;
	ChunkPos loadCentre;

private:
	void processEntities(Entity& player);
	void moveEntity(Entity& entity);
	bool blockIsCollidable(BlockPos blockPos) const;
	void loadChunks();
	void unloadChunks();
	
public:
	World(
		GlobalApplicationState& _globalApplicationState,
		const Settings& _settings
	);

	World(World&&) = delete;
	World(const World&) = delete;
	World operator=(World&&) = delete;
	World operator=(const World&) = delete;
	
	void tick(Entity& player);

	Block getBlock(BlockPos blockPos) const;
	void setBlock(BlockPos blockPos, Block block) const;
	const std::unique_ptr<Chunk>& getChunk(const ChunkPos chunkPos) const;
};