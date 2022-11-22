#include "Entity.h"



Entity::Entity(EntityPosition pos) : EntityPosition(pos), UUID{ generateUUID() } {}



long long Entity::UUIDcounter{ 0 };
long long Entity::generateUUID()
{
	return UUIDcounter++;
}
