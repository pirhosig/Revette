#pragma once
#include "StructureBoundingBox.h"
class World;



class Structure
{
public:
	Structure(unsigned long long _age, BlockPos _lowerCorner, BlockPos _upperCorner, BlockPos _centre);
	virtual ~Structure() = default;

	virtual bool canPlace(const World& world) const = 0;
	virtual void build(const World& world) = 0;

	BlockPos getCentre() const;
	StructureBoundingBox getBoundingBox() const;

protected:
	// virtual bool intersectsAny(const World& world) const;

	unsigned long long age;
	StructureBoundingBox boundingBox;
	BlockPos centre;
};