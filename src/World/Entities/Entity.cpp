#include "Entity.h"



Entity::Entity(EntityPosition pos, glm::dvec3 _size) : EntityPosition(pos), size{ _size }, UUID{ generateUUID() } {}



long long Entity::UUIDcounter{ 0 };
long long Entity::generateUUID()
{
	return UUIDcounter++;
}
