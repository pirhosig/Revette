#include "EntityPosition.h"
#include <cmath>
#include "../../Constants.h"



inline double degreesToRadians(const double angle)
{
	return angle * 0.01745329251994329576923690768489;
}



inline double wrapCoordinate(double x)
{
	if (-WORLD_RADIUS_BLOCK <= x && x < WORLD_RADIUS_BLOCK) return x;
	x = fmod(x, WORLD_DIAMETER_BLOCK);
	if (x < -WORLD_RADIUS_BLOCK)      x += WORLD_DIAMETER_BLOCK;
	else if (WORLD_RADIUS_BLOCK <= x) x -= WORLD_DIAMETER_BLOCK;
	return x;
}



EntityPosition::EntityPosition() : displacement{ 0.0, 0.0, 0.0 }
{
	setPositionRotation({ 0.0, 0.0, 0.0 }, 0.0, 0.0);
}

EntityPosition::EntityPosition(Math::Vector _pos) : displacement{ 0.0, 0.0, 0.0 }
{
	setPositionRotation(_pos, 0.0, 0.0);
}

EntityPosition::EntityPosition(Math::Vector _pos, const double xRot, const double yRot) : displacement{ 0.0, 0.0, 0.0 }
{
	setPositionRotation(_pos, xRot, yRot);
}



void EntityPosition::setPosition(Math::Vector _pos)
{
	pos.X = wrapCoordinate(_pos.X);
	pos.Y = _pos.Y;
	pos.Z = wrapCoordinate(_pos.Z);
}



void EntityPosition::setPositionRotation(Math::Vector _pos, const double xRot, const double yRot)
{
	setPosition(_pos);
	setRotation(xRot, yRot);
}



void EntityPosition::setRotation(const double xRot, const double yRot)
{
	xRotation = xRot;
	yRotation = yRot;
}



void EntityPosition::moveAbsolute(Math::Vector _offset)
{
	pos += _offset;
	wrapCoordinates();
}



void EntityPosition::displaceForward(const double distance)
{
	displacement.X += distance * std::cos(degreesToRadians(xRotation));
	displacement.Z += distance * std::sin(degreesToRadians(xRotation));
}



void EntityPosition::displaceSideways(const double distance)
{
	displacement.X += distance * std::cos(degreesToRadians(xRotation + 90.0));
	displacement.Z += distance * std::sin(degreesToRadians(xRotation + 90.0));
}



void EntityPosition::displaceVertical(const double distance)
{
	displacement.Y += distance;
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



void EntityPosition::wrapCoordinates()
{
	pos.X = wrapCoordinate(pos.X);
	pos.Z = wrapCoordinate(pos.Z);
}
