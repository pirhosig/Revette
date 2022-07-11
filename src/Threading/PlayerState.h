#pragma once
#include "../World/Entities/EntityPosition.h"



class PlayerState
{
public:
	PlayerState(EntityPosition _position) : position{ _position } {}

	EntityPosition position;
};