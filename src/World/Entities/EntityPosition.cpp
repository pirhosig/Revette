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



// Rotate the entity by the specified number of degrees
void EntityPosition::rotate(const double xRot, const double yRot)
{
	// Rotation and constrain x angle to [0, 360)
	xRotation += xRot;
	xRotation = fmod(xRotation, 360.0);
	if (xRotation < 0.0) xRotation += 360.0;

	// Rotation and constrain y angle to [-90, 90]
	yRotation += yRot;
	if (yRotation < -90.0) yRotation = -90.0;
	else if (yRotation > 90.0) yRotation = 90.0;
}
