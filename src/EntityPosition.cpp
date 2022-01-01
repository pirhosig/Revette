#include "EntityPosition.h"
#include <cmath>



inline double degreesToRadians(const double angle)
{
	return angle * 0.01745329251994329576923690768489;
}



EntityPosition::EntityPosition()
{
	setPositionRotation(0.0, 0.0, 0.0, 0.0, 0.0);
}

EntityPosition::EntityPosition(const double x, const double y, const double z)
{
	setPositionRotation(x, y, z, 0.0, 0.0);
}

EntityPosition::EntityPosition(const double x, const double y, const double z, const double xRot, const double yRot)
{
	setPositionRotation(x, y, z, xRot, yRot);
}



void EntityPosition::setPosition(const double x, const double y, const double z)
{
	X = x;
	Y = y;
	Z = z;
}



void EntityPosition::setPositionRotation(const double x, const double y, const double z, const double xRot, const double yRot)
{
	setPosition(x, y, z);
	setRotation(xRot, yRot);
}



void EntityPosition::setRotation(const double xRot, const double yRot)
{
	xRotation = xRot;
	yRotation = yRot;
}



void EntityPosition::moveAbsolute(const double x, const double y, const double z)
{
	X += x;
	Y += y;
	X += z;
}



void EntityPosition::moveForward(const double distance)
{
	X += distance * std::cos(degreesToRadians(xRotation));
	Z += distance * std::sin(degreesToRadians(xRotation));
}



void EntityPosition::moveSideways(const double distance)
{
	X += distance * std::cos(degreesToRadians(xRotation + 90.0));
	Z += distance * std::sin(degreesToRadians(xRotation + 90.0));
}



void EntityPosition::moveVertical(const double distance)
{
	Y += distance;
}
