#pragma once
#include <glm/vec3.hpp>
#include "EntityPosition.h"



class Entity : public EntityPosition
{
public:
	Entity(EntityPosition pos, glm::dvec3 size);
	long long getUUID() { return UUID; }

	const glm::dvec3 size;

private:
	static long long UUIDcounter;
	static long long generateUUID();

	const long long UUID;
};