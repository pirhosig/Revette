#pragma once
#include <glm/vec3.hpp>
#include "EntityPosition.h"



class Entity {
public:
	EntityPosition position;
	const glm::dvec3 size;

public:
	Entity(EntityPosition _position, glm::dvec3 size);
};
