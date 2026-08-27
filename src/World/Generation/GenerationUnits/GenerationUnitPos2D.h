#pragma once
#include "Core/RevetteCore.h"
#include "../../ChunkPos.h"



class GenerationUnitPos2D {
public:
	class OffsetType {
		i32 x;
		i32 z;

	public:
		OffsetType(i32 _x, i32 _z);

		bool operator==(const OffsetType&) const = default;
		i32 getX() const;
		i32 getZ() const;
	};



private:
	i32 x;
	i32 z;



public:
	GenerationUnitPos2D(i32 _x, i32 _z);
	GenerationUnitPos2D(ChunkPos2D pos);

	bool operator==(const GenerationUnitPos2D&) const = default;
	i32 getX() const;
	i32 getZ() const;

	i32 getManhattanDistance(const GenerationUnitPos2D& other) const;
	OffsetType getOffsetTo(const GenerationUnitPos2D& other) const;
	GenerationUnitPos2D calculateOffsetBy(i32 xDistance, i32 zDistance) const;

};
