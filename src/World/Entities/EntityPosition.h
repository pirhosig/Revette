#pragma once
#include "../../Math/Math.h"



class EntityPosition
{
public:
	EntityPosition();
	EntityPosition(Math::Vector _pos);
	EntityPosition(Math::Vector _pos, const double xRot, const double yRot);

	void setPosition(Math::Vector _pos);
	void setPositionRotation(Math::Vector _pos, const double xRot, const double yRot);
	void setRotation(const double xRot, const double yRot);
	void moveAbsolute(Math::Vector _offset);
	void displaceForward(const double distance);
	void displaceSideways(const double distance);
	void displaceVertical(const double distance);
	void rotate(const double xRot, const double yRot);

	Math::Vector pos;
	Math::Vector displacement;
	double xRotation;
	double yRotation;

private:
	void wrapCoordinates();
};
