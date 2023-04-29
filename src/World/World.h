#pragma once
#include <map>

#include "Block.h"
#include "Chunk.h"
#include "ChunkStatusMap.h"
#include "Entities/Entity.h"
#include "Generation/GeneratorChunkParameters.h"
#include "Generation/GeneratorChunkNoise.h"
#include "Generation/Structures/Structure.h"
#include "../Rendering/Mesh/MeshDataChunk.h"
#include "../Threading/ThreadPointerQueue.h"



class ChunkPriorityTicket
{
public:
	int priority;
	ChunkPos pos;

	ChunkPriorityTicket(int _priority, ChunkPos _pos) : priority{ _priority }, pos{ _pos } {}

	bool operator<(const ChunkPriorityTicket& other) const
	{
		if (priority != other.priority) return priority < other.priority;
		else return pos < other.pos;
	}
};



class World
{
public:
	World(
		std::shared_ptr<ThreadPointerQueue<MeshDataChunk>> queueMesh,
		std::shared_ptr<ThreadQueue<ChunkPos>> queueMeshDeletion,
		const char* settingNoiseHeightmap
	);
	World(const World&) = delete;
	void tick(Entity& player);

	Block getBlock(BlockPos blockPos) const;
	void setBlock(BlockPos blockPos, Block block) const;
	const std::unique_ptr<Chunk>& getChunk(const ChunkPos chunkPos) const;

	void addStructure(const BlockPos _blockPos, std::unique_ptr<Structure> _structure);
	const std::unique_ptr<Structure>& getStructure(const BlockPos blockPos) const;

private:
	void processEntities(Entity& player);
	void moveEntity(Entity& entity);
	bool collides(BlockPos blockPos) const;

	void onLoadCentreChange();

	void updateLoadQueue();
	void loadChunks();
	void populateChunks();
	void meshChunks();


	void queueChunkMeshing(const ChunkPos chunkPos);
	void queueChunkPopulation(const ChunkPos chunkPos);

	const GeneratorChunkParameters& getGeneratorChunkParameters(const ChunkPos2D position);

	// Chunk storage
	std::map<long long, Entity> mapEntities;
	std::map<ChunkPos, std::unique_ptr<Chunk>> mapChunks;
	std::map<BlockPos, std::unique_ptr<Structure>> mapStructures;

	// Chunk loading information
	ChunkPos loadCentre;
	ChunkStatusMap chunkStatusMap;
	std::priority_queue<ChunkPriorityTicket> loadQueue;
	std::priority_queue<ChunkPriorityTicket> populateQueue;
	std::priority_queue<ChunkPriorityTicket> meshQueue;

	// Chunk generation tools
	std::map<ChunkPos2D, GeneratorChunkParameters> generatorChunkCache;
	GeneratorChunkNoise generatorChunkNoise;

	std::shared_ptr<ThreadPointerQueue<MeshDataChunk>> threadQueueMeshes;
	std::shared_ptr<ThreadQueue<ChunkPos>> threadQueueMeshDeletion;
};