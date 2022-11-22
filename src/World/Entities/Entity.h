#pragma once
#include "EntityPosition.h"


class Entity : public EntityPosition
{
public:
	Entity(EntityPosition pos);
	long long getUUID() { return UUID; }

private:
	static long long UUIDcounter;
	static long long generateUUID();

	const long long UUID;
};