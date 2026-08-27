#include "World.h"
#include <algorithm>
#include <cassert>
#include <cmath>

#include "Core/RevetteCore.h"
#include "Physics.h"
#include "../Exceptions.h"
#include "../GlobalLog.h"



namespace {

inline i32 sign(double x) {
	return static_cast<i32>(0.0 < x) - static_cast<i32>(x < 0.0);
}

}



World::GlobalStateType::GlobalStateType() {
	incomingChunks.reserve(1024U);
}



World::GlobalStateType World::globalState{};



World::World(
	GlobalApplicationState& _globalApplicationState,
	const Settings& _settings
) :
	globalApplicationState{_globalApplicationState},
	settings{_settings},
	chunkUnloadDistanceSquared{
		static_cast<i64>(settings.getLoadDistanceHorizontal() + 5) * (settings.getLoadDistanceHorizontal() + 5)
	},
	loadCentre(0, 1, 0)
{
	GlobalLog.Write("Loaded World");
}



void World::tick(Entity& player) {
	if (ChunkPos playerChunkPos(player.position);
		playerChunkPos != loadCentre
	) {
		loadCentre = playerChunkPos;
		globalApplicationState.playerChunkPosition.store(playerChunkPos);
		unloadChunks();
	}

	loadChunks();

	processEntities(player);
}



Block World::getBlock(BlockPos blockPos) const {
	return getChunk(ChunkPos(blockPos))->getBlock(ChunkLocalBlockPos(blockPos));
}



void World::setBlock(BlockPos blockPos, Block block) const {
	getChunk(ChunkPos(blockPos))->setBlock(ChunkLocalBlockPos(blockPos), block);
}



void World::processEntities(Entity& player) {
	moveEntity(player);
	for (auto& [UUID, entity] : mapEntities) {
		moveEntity(entity);
	}
}



void World::moveEntity(Entity& entity) {
	EntityPosition& pos = entity.position;

	if (pos.displacement.x == 0.0 && pos.displacement.y == 0.0 && pos.displacement.z == 0.0) {
		return;
	}

	BlockPos currentPos(pos.pos.x - entity.size.x, pos.pos.y, pos.pos.z - entity.size.z);

	const double _DX = std::clamp(pos.displacement.x, -32.0, 32.0);
	const double _DY = std::clamp(pos.displacement.y, -32.0, 32.0);
	const double _DZ = std::clamp(pos.displacement.z, -32.0, 32.0);

	const auto stepX = sign(_DX);
	const auto stepY = sign(_DY);
	const auto stepZ = sign(_DZ);

	const double tDeltaX = std::clamp(1.0 / std::abs(_DX), 0.0, 1.0);
	const double tDeltaY = std::clamp(1.0 / std::abs(_DY), 0.0, 1.0);
	const double tDeltaZ = std::clamp(1.0 / std::abs(_DZ), 0.0, 1.0);

	double tMaxX = stepX ? 
		((0 < stepX) ? std::ceil(pos.pos.x) - pos.pos.x : pos.pos.x - std::floor(pos.pos.x)) * tDeltaX :
		1.0;
	double tMaxY = stepY ?
		((0 < stepY) ? std::ceil(pos.pos.y) - pos.pos.y : pos.pos.y - std::floor(pos.pos.y)) * tDeltaY :
		1.0;
	double tMaxZ = stepZ ?
		((0 < stepZ) ? std::ceil(pos.pos.z) - pos.pos.z : pos.pos.z - std::floor(pos.pos.z)) * tDeltaZ :
		1.0;

	const auto _sx = static_cast<i32>(std::ceil(entity.size.x * 2)) - 1;
	const auto _sy = static_cast<i32>(std::ceil(entity.size.y))     - 1;
	const auto _sz = static_cast<i32>(std::ceil(entity.size.z * 2)) - 1;

	while (tMaxX < 1.0 || tMaxY < 1.0 || tMaxZ < 1.0) {
		if (tMaxX < tMaxY) {
			if (tMaxX < tMaxZ) {
				i32 lX = (stepX > 0) ? _sx : 0;
				for (i32 lZ = 0; lZ < _sz; ++lZ) {
				for (i32 lY = 0; lY < _sy; ++lY) {
					if (blockIsCollidable(currentPos.offset(stepX + lX, lY, lZ))) goto Collided;
				}
				}
				tMaxX += tDeltaX;
				currentPos = currentPos.offset(stepX, 0, 0);
			}
			else {
				i32 lZ = (stepZ > 0) ? _sz : 0;
				for (i32 lY = 0; lY < _sy; ++lY) {
				for (i32 lX = 0; lX < _sx; ++lX) {
					if (blockIsCollidable(currentPos.offset(lX, lY, stepZ + lZ))) goto Collided;
				}
				}
				tMaxZ += tDeltaZ;
				currentPos = currentPos.offset(0, 0, stepZ);
			}
		}
		else if (tMaxY < tMaxZ) {
			i32 lY = (stepY > 0) ? _sy : 0;
			for (i32 lX = 0; lX < _sx; ++lX) {
			for (i32 lZ = 0; lZ < _sz; ++lZ) {
				if (blockIsCollidable(currentPos.offset(lX, stepY + lY, lZ))) goto Collided;
			}
			}
			tMaxY += tDeltaY;
			currentPos = currentPos.offset(0, stepY, 0);
		}
		else {
			i32 lZ = (stepZ > 0) ? _sz : 0;
			for (i32 lY = 0; lY < _sy; ++lY) {
			for (i32 lX = 0; lX < _sx; ++lX) {
				if (blockIsCollidable(currentPos.offset(lX, lY, stepZ + lZ))) goto Collided;
			}
			}
			tMaxZ += tDeltaZ;
			currentPos = currentPos.offset(0, 0, stepZ);
		}
	}

Collided:
	double tTotal = std::clamp(std::min(tMaxX, std::min(tMaxY, tMaxZ)), 0.0, 1.0);
	pos.moveAbsolute({ _DX * tTotal, _DY * tTotal, _DZ * tTotal });
	pos.displacement = { 0.0, 0.0, 0.0 };
}



bool World::blockIsCollidable(BlockPos blockPos) const {
	auto it = mapChunks.find(ChunkPos(blockPos));
	if (it == mapChunks.end()) {
		return true;
	}
	return Physics::IS_COLLIDABLE[getBlock(blockPos).blockType];
}



void World::loadChunks() {
	std::scoped_lock lock(World::globalState.mutexForIncomingChunks);
	for (auto& chunk : World::globalState.incomingChunks) {
		const auto _pos = chunk->getPosition();
		if (mapChunks.try_emplace(
			_pos,
			std::move(chunk)
		).second == false) {
			throw std::runtime_error("in function World::loadChunks(): Chunk unexpectedly recreated.");
		}
	}
}



void World::unloadChunks() {
	std::erase_if(
		mapChunks,
		[&](const auto& item) {
			const auto& [_pos, _chunk] = item;
			return (
				(loadCentre.distanceEuclideanSquared(_pos) > chunkUnloadDistanceSquared) ||
				(std::abs(loadCentre.getY() - _pos.getY()) > settings.getLoadDistanceVertical() + 5)
			);
		}
	);
}



// Returns a reference to a chunk
const std::unique_ptr<Chunk>& World::getChunk(const ChunkPos chunkPos) const {
	try {
		return mapChunks.at(chunkPos);
	}
	catch (const std::out_of_range& e)
	{
		std::string error = "Attempted to access non-existent chunk at ";
		error += std::to_string(chunkPos.getX()) + " " + std::to_string(chunkPos.getY()) + " " + std::to_string(chunkPos.getZ());
		throw EXCEPTION_WORLD::ChunkNonExistence(error);
	}
}
