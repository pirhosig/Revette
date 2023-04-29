#include "World.h"
#include <cassert>
#include <cmath>

#include "Physics.h"
#include "../Exceptions.h"



constexpr int SEED = 69;



inline bool withinGenerationDistance(ChunkPos _pos, ChunkPos _centre)
{
	auto _offset = _pos.offset(_centre);
	return (std::abs(_offset.x) <= LOAD_DISTANCE &&
			std::abs(_offset.y) <= LOAD_DISTANCE_VERTICAL &&
			std::abs(_offset.z) <= LOAD_DISTANCE);
}



inline bool withinGenerationDistance2D(ChunkPos _pos, ChunkPos _centre)
{
	auto _offset = _pos.offset(_centre);
	return (std::abs(_offset.x) <= LOAD_DISTANCE && std::abs(_offset.z) <= LOAD_DISTANCE);
}



inline bool withinPopulationDistance(ChunkPos _pos, ChunkPos _centre)
{
	auto _offset = _pos.offset(_centre);
	return (std::abs(_offset.x) < LOAD_DISTANCE &&
			std::abs(_offset.y) < LOAD_DISTANCE_VERTICAL &&
			std::abs(_offset.z) < LOAD_DISTANCE);
}



inline bool withinMeshingDistance(ChunkPos _pos, ChunkPos _centre)
{
	auto _offset = _pos.offset(_centre);
	return (std::abs(_offset.x) < LOAD_DISTANCE - 1 &&
		std::abs(_offset.y) < LOAD_DISTANCE_VERTICAL - 1 &&
		std::abs(_offset.z) < LOAD_DISTANCE - 1);
}




World::World(
	std::shared_ptr<ThreadPointerQueue<MeshDataChunk>> queueMesh,
	std::shared_ptr<ThreadQueue<ChunkPos>> queueMeshDeletion,
	const char* settingNoiseHeightmap
) :
	loadCentre(0, 0, 0),
	threadQueueMeshes{ queueMesh },
	threadQueueMeshDeletion{ queueMeshDeletion },
	generatorChunkNoise(
		SEED,
		0.03125f,
		0.015625f,
		0.015625f,
		settingNoiseHeightmap,
		"HAABGQANAAIAAAAAAABAEwAK16M8CQAAAAAAPwAAAAAAAAAAgD8AAAAAQA==",
		"HAABGQANAAIAAAAAAABAEwAK16M8CQAAAAAAPwAAAAAAAAAAgD8AAAAAQA==",
		"HAABBgAAAABAQA=="
	)
{
	updateLoadQueue();
}



void World::tick(Entity& player)
{
	

	processEntities(player);


	ChunkPos _playerChunk(player);
	if (_playerChunk != loadCentre)
	{
		loadCentre = _playerChunk;
		onLoadCentreChange();
	}

	{
		std::queue<ChunkPos> meshUnloadQueue;
		threadQueueMeshDeletion->getQueue(meshUnloadQueue);
		while (!meshUnloadQueue.empty())
		{
			ChunkPos _pos = meshUnloadQueue.front();
			meshUnloadQueue.pop();
			if (chunkStatusMap.chunkExists(_pos)) chunkStatusMap.setChunkStatusMesh(_pos, StatusChunkMesh::NON_EXISTENT);
		}
	}

	loadChunks();
	populateChunks();
	meshChunks();
}



Block World::getBlock(BlockPos blockPos) const
{
	return getChunk(ChunkPos(blockPos))->getBlock(ChunkLocalBlockPos(blockPos));
}



void World::setBlock(BlockPos blockPos, Block block) const
{
	getChunk(ChunkPos(blockPos))->setBlock(ChunkLocalBlockPos(blockPos), block);
}



void World::processEntities(Entity& player)
{
	moveEntity(player);

	for (auto& [UUID, entity] : mapEntities)
	{
		moveEntity(entity);
	}
}



inline double getFracAbs(double x)
{
	double _pI;
	return std::abs(std::modf(x, &_pI));
}



void World::moveEntity(Entity& entity)
{
	constexpr double SMALL = 1e-6;

	// Skip if entity has zero velocity
	if (entity.displacement.X == 0.0 && entity.displacement.Y == 0 && entity.displacement.Z == 0) return;

	// Try to process the movement this tick
	double _DX = entity.displacement.X;
	double _DY = entity.displacement.Y;
	double _DZ = entity.displacement.Z;

	// Check if the entity is initially pressed against any voxel faces
	if (std::abs(_DX) > SMALL) {
		double _rX = getFracAbs(_DX > 0 ? entity.pos.X + 1.0 : entity.pos.X - 1.0);
		if ((_rX < SMALL) || (_rX > (1.0 - SMALL))) {
			int _lowZ = static_cast<int>(std::floor(entity.pos.Z - 1.0));
			int _uppZ = static_cast<int>(std::ceil(entity.pos.Z + 1.0));
			int _lowY = static_cast<int>(std::floor(entity.pos.Y));
			int _uppY = static_cast<int>(std::ceil(entity.pos.Y + 4.0));
			int _pX = _DX > 0.0 ? static_cast<int>(std::floor(entity.pos.X + 1.0)) :
				static_cast<int>(std::floor(entity.pos.X - 1.0));
			for (int lZ = _lowZ; lZ < _uppZ; ++lZ) {
				for (int lY = _lowY; lY < _uppY; ++lY) {
					if (collides(BlockPos(_pX, lY, lZ))) _DX = 0.0;
				}
			}
		}
	}

	if (std::abs(_DZ) > SMALL) {
		double _rZ = getFracAbs(_DX > 0 ? entity.pos.Z + 1.0 : entity.pos.Z - 1.0);
		if ((_rZ < SMALL) || (_rZ > (1.0 - SMALL))) {
			int _lowX = static_cast<int>(std::floor(entity.pos.X - 1.0));
			int _uppX = static_cast<int>(std::ceil(entity.pos.X + 1.0));
			int _lowY = static_cast<int>(std::floor(entity.pos.Y));
			int _uppY = static_cast<int>(std::ceil(entity.pos.Y + 4.0));
			int _pZ = static_cast<int>(_DZ > 0.0 ? std::floor(entity.pos.Z + 1.0) : std::floor(entity.pos.Z - 1.0));
			for (int lX = _lowX; lX < _uppX; ++lX) {
				for (int lY = _lowY; lY < _uppY; ++lY)
					if (collides(BlockPos(lX, lY, _pZ))) _DZ = 0.0;
			}
		}
	}


	if (std::abs(_DY) > SMALL) {
		double _rY = getFracAbs(_DY > 0 ? entity.pos.Y + 4.0 : entity.pos.Y);
		if ((_rY < SMALL) || (_rY > (1.0 - SMALL))) {
			int _lowX = static_cast<int>(std::floor(entity.pos.X - 1.0));
			int _uppX = static_cast<int>(std::ceil(entity.pos.X + 1.0));
			int _lowZ = static_cast<int>(std::floor(entity.pos.Z - 1.0));
			int _uppZ = static_cast<int>(std::ceil(entity.pos.Z + 1.0));
			int _pY = static_cast<int>(_DY > 0.0 ? std::floor(entity.pos.Y + 4.0) : std::floor(entity.pos.Y));
			for (int lX = _lowX; lX < _uppX; ++lX) {
				for (int lZ = _lowZ; lZ < _uppZ; ++lZ)
					if (collides(BlockPos(lX, _pY, lZ))) _DY = 0.0;
			}
		}
	}

	// Loop while movement remains
	while (std::abs(_DX) > SMALL || std::abs(_DY) > SMALL || std::abs(_DZ) > SMALL)
	{
		// Calculate first voxel edge crossing to occur
		double _collisionTimeX = std::abs(_DX) > SMALL ?
			(1.0 - getFracAbs(_DX > 0 ? entity.pos.X + 1.0 : entity.pos.X - 1.0)) / std::abs(_DX) : 1.0;
		double _collisionTimeY = std::abs(_DY) > SMALL ?
			(1.0 - getFracAbs(_DY > 0 ? entity.pos.Y + 4.0 : entity.pos.Y)) / std::abs(_DY) : 1.0;
		double _collisionTimeZ = std::abs(_DZ) > SMALL ?
			(1.0 - getFracAbs(_DZ > 0 ? entity.pos.Z + 1.0 : entity.pos.Z - 1.0)) / std::abs(_DZ) : 1.0;
		double _minCollisionTime = std::min(std::min(_collisionTimeX, _collisionTimeZ), _collisionTimeY);

		// If no voxel border crossing occurs, simply move all of the remaining velocity
		if (_minCollisionTime >= 1.0)
		{
			entity.moveAbsolute({ _DX, _DY, _DZ });
			break;
		}
		// Handle collision checking
		else
		{
			// Move by time before collision
			entity.moveAbsolute(Math::Vector{ _DX, _DY, _DZ } *_minCollisionTime);
			_DX -= _minCollisionTime * _DX;
			_DY -= _minCollisionTime * _DY;
			_DZ -= _minCollisionTime * _DZ;

			if (_collisionTimeX == _minCollisionTime && std::abs(_DX) > SMALL) {
				int _lowZ = static_cast<int>(std::floor(entity.pos.Z - 1.0));
				int _uppZ = static_cast<int>(std::ceil(entity.pos.Z + 1.0));
				int _lowY = static_cast<int>(std::floor(entity.pos.Y));
				int _uppY = static_cast<int>(std::ceil(entity.pos.Y + 4.0));
				int _pX = static_cast<int>(_DX > 0.0 ? std::ceil(entity.pos.X + 1.0) : std::floor(entity.pos.X - 1.0));
				for (int lZ = _lowZ; lZ < _uppZ; ++lZ) {
					for (int lY = _lowY; lY < _uppY; ++lY)
						if (collides(BlockPos(_pX, lY, lZ))) _DX = 0.0;
				}
			}
			if (_collisionTimeZ == _minCollisionTime && std::abs(_DZ) > SMALL)
			{
				int _lowX = static_cast<int>(std::floor(entity.pos.X - 1.0));
				int _uppX = static_cast<int>(std::ceil(entity.pos.X + 1.0));
				int _lowY = static_cast<int>(std::floor(entity.pos.Y));
				int _uppY = static_cast<int>(std::ceil(entity.pos.Y + 4.0));
				int _pZ = static_cast<int>(_DZ > 0.0 ? std::ceil(entity.pos.Z + 1.0) : std::floor(entity.pos.Z - 1.0));
				for (int lX = _lowX; lX < _uppX; ++lX) {
					for (int lY = _lowY; lY < _uppY; ++lY)
						if (collides(BlockPos(lX, lY, _pZ))) _DZ = 0.0;
				}
			}
			if (_collisionTimeY == _minCollisionTime && std::abs(_DY) > SMALL)
			{
				int _lowX = static_cast<int>(std::floor(entity.pos.X - 1.0));
				int _uppX = static_cast<int>(std::ceil(entity.pos.X + 1.0));
				int _lowZ = static_cast<int>(std::floor(entity.pos.Z - 1.0));
				int _uppZ = static_cast<int>(std::ceil(entity.pos.Z + 1.0));
				int _pY = static_cast<int>(_DY > 0.0 ? std::ceil(entity.pos.Y + 4.0) : std::floor(entity.pos.Y));
				for (int lX = _lowX; lX < _uppX; ++lX) {
					for (int lZ = _lowZ; lZ < _uppZ; ++lZ)
						if (collides(BlockPos(lX, _pY, lZ))) _DY = 0.0;
				}
			}
		}
	}

	entity.displacement = { 0.0, 0.0, 0.0 };
}



bool World::collides(BlockPos blockPos) const
{
	return Physics::IS_COLLIDABLE[getBlock(blockPos).blockType];
}



void World::onLoadCentreChange()
{
	// Jesus christ this function might just be hands down one of the worst pieces of code I have ever written
	// There is an unbelievable amount of things that could be optimised, done better or probably done without
	// I pray to god that this never breaks because I sure as hell do not know how it works.
	// Update: well fuck, it doesn't quite work

	// Clear all chunk queues
	loadQueue = std::priority_queue<ChunkPriorityTicket>();
	populateQueue = std::priority_queue<ChunkPriorityTicket>();
	meshQueue = std::priority_queue<ChunkPriorityTicket>();

	// Remove all unneeded chunks
	std::vector<ChunkPos> unloadQueue;

	for (auto& status : chunkStatusMap.statusMap)
	{
		auto& _pos = status.first;
		if (!withinGenerationDistance(_pos, loadCentre)) unloadQueue.push_back(_pos);
	}

	for (auto& _pos : unloadQueue)
	{
		chunkStatusMap.setChunkStatusLoad(_pos, StatusChunkLoad::NON_EXISTENT);
		mapChunks.erase(_pos);

		// Check if cached generation data can be cleared
		if (!withinGenerationDistance2D(_pos, loadCentre)) generatorChunkCache.erase(ChunkPos2D(_pos));
	}

	// Figure out which chunks need loading
	for (int lX = loadCentre.x - LOAD_DISTANCE; lX <= loadCentre.x + LOAD_DISTANCE; ++lX)
	{
		for (int lZ = loadCentre.z - LOAD_DISTANCE; lZ <= loadCentre.z + LOAD_DISTANCE; ++lZ)
		{
			for (int lY = loadCentre.y - LOAD_DISTANCE_VERTICAL; lY <= loadCentre.y + LOAD_DISTANCE_VERTICAL; ++lY)
			{
				auto _pos = ChunkPos(lX, lY, lZ);
				auto _status = chunkStatusMap.getChunkStatusLoad(_pos);
				int _priority = std::max(100 - static_cast<int>(loadCentre.distance(_pos)), 0);
				if (_status == StatusChunkLoad::NON_EXISTENT)
				{
					// Queue for loading
					chunkStatusMap.setChunkStatusLoad(_pos, StatusChunkLoad::QUEUED_LOAD);
					loadQueue.push(ChunkPriorityTicket(_priority, _pos));
				}
				// Add back to load queue
				else if (_status == StatusChunkLoad::QUEUED_LOAD) loadQueue.push(ChunkPriorityTicket(_priority, _pos));
			}
		}
	}

	// Figure out which chunks need populating
	for (int lX = loadCentre.x - LOAD_DISTANCE + 1; lX < loadCentre.x + LOAD_DISTANCE; ++lX)
	{
		for (int lZ = loadCentre.z - LOAD_DISTANCE + 1; lZ < loadCentre.z + LOAD_DISTANCE; ++lZ)
		{
			for (int lY = loadCentre.y - LOAD_DISTANCE_VERTICAL + 1; lY < loadCentre.y + LOAD_DISTANCE_VERTICAL; ++lY)
			{
				auto _pos = ChunkPos(lX, lY, lZ);
				auto _status = chunkStatusMap.getChunkStatusLoad(_pos);
				int _priority = std::max(100 - static_cast<int>(loadCentre.distance(_pos)), 0);
				if (_status == StatusChunkLoad::GENERATED)
				{
					// Check if population is possible
					if (chunkStatusMap.getChunkStatusCanPopulate(_pos))
					{
						chunkStatusMap.setChunkStatusLoad(_pos, StatusChunkLoad::QUEUED_POPULATE);
						populateQueue.push(ChunkPriorityTicket(_priority, _pos));
					}
				}
				// Add back to population queue
				else if (_status == StatusChunkLoad::QUEUED_POPULATE) populateQueue.push(ChunkPriorityTicket(_priority, _pos));
			}
		}
	}

	// Figure out wtf is going on with meshing
	for (int lX = loadCentre.x - LOAD_DISTANCE; lX <= loadCentre.x + LOAD_DISTANCE; ++lX)
	{
		for (int lZ = loadCentre.z - LOAD_DISTANCE; lZ <= loadCentre.z + LOAD_DISTANCE; ++lZ)
		{
			for (int lY = loadCentre.y - LOAD_DISTANCE_VERTICAL; lY <= loadCentre.y + LOAD_DISTANCE_VERTICAL; ++lY)
			{
				auto _pos = ChunkPos(lX, lY, lZ);
				auto _status = chunkStatusMap.getChunkStatusMesh(_pos);
				if (!withinMeshingDistance(_pos, loadCentre))
				{
					if (_status != StatusChunkMesh::NON_EXISTENT) chunkStatusMap.setChunkStatusMesh(_pos, StatusChunkMesh::NON_EXISTENT);
				}
				else
				{
					int _priority = std::max(100 - static_cast<int>(loadCentre.distance(_pos)), 0);
					switch (_status)
					{
					case StatusChunkMesh::NON_EXISTENT:
						if (chunkStatusMap.getChunkStatusCanMesh(_pos))
						{
							meshQueue.push(ChunkPriorityTicket(_priority, _pos));
							chunkStatusMap.setChunkStatusMesh(_pos, StatusChunkMesh::QUEUED);
						}
						break;
					case StatusChunkMesh::QUEUED:
						meshQueue.push(ChunkPriorityTicket(_priority, _pos));
						break;
					case StatusChunkMesh::MESHED:
						break;
					default:
						break;
					}
				}
			}
		}
	}
}



// Updates the load queue by removing chunks too far away, updating priorities and adding chunks within load distance
void World::updateLoadQueue()
{
	// Update priority of queued chunks
	{
		std::priority_queue<ChunkPriorityTicket> updatedQueue;

		while (!loadQueue.empty())
		{
			const ChunkPriorityTicket item = loadQueue.top();
			loadQueue.pop();
			auto& _pos = item.pos;
			// Check if chunk still falls within load distance
			if (withinGenerationDistance(_pos, loadCentre))
			{
				// Add chunk back with updated priority
				int priority = std::max(100 - static_cast<int>(loadCentre.distance(_pos)), 0);
				updatedQueue.push(ChunkPriorityTicket(priority, _pos));
			}
			// Skip chunk and remove queued status
			else chunkStatusMap.setChunkStatusLoad(_pos, StatusChunkLoad::NON_EXISTENT);
		}

		std::swap(loadQueue, updatedQueue);
	}

	for (int x = loadCentre.x - LOAD_DISTANCE; x <= loadCentre.x + LOAD_DISTANCE; ++x)
	{
		for (int z = loadCentre.z - LOAD_DISTANCE; z <= loadCentre.z + LOAD_DISTANCE; ++z)
		{
			for (int y = loadCentre.y - LOAD_DISTANCE_VERTICAL; y <= loadCentre.y + LOAD_DISTANCE_VERTICAL; ++y)
			{
				ChunkPos loadPos(x, y, z);
				// Make sure chunks queued to be loaded do not exist yet
				if (chunkStatusMap.getChunkStatusLoad(loadPos) != StatusChunkLoad::NON_EXISTENT) continue;
				int priority = std::max(100 - static_cast<int>(loadCentre.distance(loadPos)), 0);
				// Update chunk state
				chunkStatusMap.setChunkStatusLoad(loadPos, StatusChunkLoad::QUEUED_LOAD);
				loadQueue.push(ChunkPriorityTicket(priority, loadPos));
			}
		}
	}
}



void World::loadChunks()
{
	constexpr int MAX_LOAD_COUNT = 25;
	for (int i = 0; i < MAX_LOAD_COUNT; ++i)
	{
		if (loadQueue.empty()) break;
		ChunkPos lPos = loadQueue.top().pos;
		loadQueue.pop();

		// Make sure that the chunk is queued for loading (something has gone horribly wrong if it isn't)
		assert((chunkStatusMap.getChunkStatusLoad(lPos) == StatusChunkLoad::QUEUED_LOAD) && "Attempted to load already loaded chunk.");

		// Load the chunk
		auto insertRes = mapChunks.insert({ lPos, std::make_unique<Chunk>(lPos) });
		chunkStatusMap.setChunkStatusLoad(lPos, StatusChunkLoad::LOADED);

		// Generate the chunk
		insertRes.first->second->GenerateChunk(getGeneratorChunkParameters(ChunkPos2D(lPos)));
		chunkStatusMap.setChunkStatusLoad(lPos, StatusChunkLoad::GENERATED);
		// Check if it, or its neighbours can populate
		for (int ni = -1; ni < 2; ++ni) {
			for (int nj = -1; nj < 2; ++nj) {
				for (int nk = -1; nk < 2; ++nk) {
					ChunkPos _pos(lPos.x + ni, lPos.y + nj, lPos.z + nk);
					if (withinPopulationDistance(_pos, loadCentre) && chunkStatusMap.getChunkStatusCanPopulate(_pos))
						queueChunkPopulation(_pos);
				}
			}
		}
	}
}



void World::populateChunks()
{
	constexpr int MAX_POPULATE_COUNT = 25;
	for (int i = 0; !populateQueue.empty() && i < MAX_POPULATE_COUNT; ++i)
	{
		ChunkPos _pos = populateQueue.top().pos;
		populateQueue.pop();

		// Make sure that the chunk is queued for population
		assert(withinPopulationDistance(_pos, loadCentre) && (chunkStatusMap.getChunkStatusLoad(_pos) == StatusChunkLoad::QUEUED_POPULATE) && "Attempted to populate already populated chunk.");

		getChunk(_pos)->PopulateChunk(*this);

		chunkStatusMap.setChunkStatusLoad(_pos, StatusChunkLoad::POPULATED);
		// Check if this chunk or any cardinal neighbours can generate meshes
		if (chunkStatusMap.getChunkStatusCanMesh(_pos)) queueChunkMeshing(_pos);

		const int NEIGHBOURS[6][3] = {
			{  1,  0,  0 },
			{ -1,  0,  0 },
			{  0,  1,  0 },
			{  0, -1,  0 },
			{  0,  0,  1 },
			{  0,  0, -1 },
		};
		for (auto [_dx, _dy, _dz] : NEIGHBOURS)
		{
			ChunkPos meshPos(_pos.x + _dx, _pos.y + _dy, _pos.z + _dz);
			if (chunkStatusMap.getChunkStatusCanMesh(meshPos)) queueChunkMeshing(meshPos);
		}
	}
}



void World::meshChunks()
{
	std::queue<std::unique_ptr<MeshDataChunk>> meshDataQueue;

	constexpr int MAX_MESH_COUNT = 10;
	for (int i = 0; i < MAX_MESH_COUNT; ++i)
	{
		if (meshQueue.empty()) break;
		ChunkPos mPos = meshQueue.top().pos;
		meshQueue.pop();

		// Make sure chunk is generated but does not have a mesh (the universe is broken if it isn't)
		assert((chunkStatusMap.getChunkStatusLoad(mPos) == StatusChunkLoad::POPULATED) && "Attempted to create mesh for chunk that has not finished loading");
		assert((chunkStatusMap.getChunkStatusMesh(mPos) == StatusChunkMesh::QUEUED) && "Attempted to regenerate mesh.");

		// Create a mesh if the chunk is not empty
		if (!getChunk(mPos)->isEmpty()) {
			std::unique_ptr<MeshDataChunk> meshData = std::make_unique<MeshDataChunk>(
				mPos,
				getChunk(mPos),
				getChunk(mPos.direction(AxisDirection::Up)),
				getChunk(mPos.direction(AxisDirection::Down)),
				getChunk(mPos.direction(AxisDirection::North)),
				getChunk(mPos.direction(AxisDirection::South)),
				getChunk(mPos.direction(AxisDirection::East)),
				getChunk(mPos.direction(AxisDirection::West))
				);
			if (meshData->triangleCount > 0) meshDataQueue.push(std::move(meshData));
		}
		chunkStatusMap.setChunkStatusMesh(mPos, StatusChunkMesh::MESHED);
	}

	// Push meshes, if any were created
	if (meshDataQueue.size()) threadQueueMeshes->mergeQueue(meshDataQueue);
}



// Returns a reference to a chunk
const std::unique_ptr<Chunk>& World::getChunk(const ChunkPos chunkPos) const
{
	try {
		return mapChunks.at(chunkPos);
	}
	catch (std::out_of_range)
	{
		std::string error = "Attempted to access non-existent chunk at ";
		error += std::to_string(chunkPos.x) + " " + std::to_string(chunkPos.y) + " " + std::to_string(chunkPos.z);
		throw EXCEPTION_WORLD::ChunkNonExistence(error);
	}
}



void World::addStructure(const BlockPos _blockPos, std::unique_ptr<Structure> _structure)
{
	mapStructures[_blockPos] = std::move(_structure);
}



// Returns a reference to the structure at the location
const std::unique_ptr<Structure>& World::getStructure(const BlockPos blockPos) const
{
	try {
		return mapStructures.at(blockPos);
	}
	catch (std::out_of_range)
	{
		std::string error = "Attempted to access non-existent chunk at ";
		error += std::to_string(blockPos.x) + " " + std::to_string(blockPos.y) + " " + std::to_string(blockPos.z);
		throw EXCEPTION_WORLD::StructureNonExistence(error);
	}
}



void World::queueChunkMeshing(const ChunkPos chunkPos)
{
	assert(chunkStatusMap.getChunkStatusCanMesh(chunkPos) && "Attempted to queue mesh that cannot be meshed");
	int meshPriority = std::max(100 - static_cast<int>(loadCentre.distance(chunkPos)), 0);
	meshQueue.push(ChunkPriorityTicket(meshPriority, chunkPos));
	chunkStatusMap.setChunkStatusMesh(chunkPos, StatusChunkMesh::QUEUED);
}



void World::queueChunkPopulation(const ChunkPos chunkPos)
{
	assert(withinPopulationDistance(chunkPos, loadCentre) && chunkStatusMap.getChunkStatusCanPopulate(chunkPos) &&
		"Attempted to populate chunk that cannot be populated");
	int priority = std::max(100 - static_cast<int>(loadCentre.distance(chunkPos)), 0);
	populateQueue.push(ChunkPriorityTicket(priority, chunkPos));
	chunkStatusMap.setChunkStatusLoad(chunkPos, StatusChunkLoad::QUEUED_POPULATE);
}




const GeneratorChunkParameters& World::getGeneratorChunkParameters(const ChunkPos2D position)
{
	if (!generatorChunkCache.contains(position)) generatorChunkCache.try_emplace(position, position, generatorChunkNoise);
	return generatorChunkCache.at(position);
}