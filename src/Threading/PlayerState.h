#pragma once
#include "../World/Entities/EntityPosition.h"



class PlayerState
{
public:
	PlayerState(EntityPosition _position) :
		position{ _position },
		velocityX{ 0.0 },
		velocityY{ 0.0 },
		velocityZ{ 0.0 }
	{}

	EntityPosition position;

	double velocityX;
	double velocityY;
	double velocityZ;
};