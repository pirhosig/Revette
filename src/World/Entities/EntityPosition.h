#pragma once
#include <glm/vec3.hpp>



class EntityPosition
{
public:
	EntityPosition();
	EntityPosition(glm::dvec3 _pos);
	EntityPosition(glm::dvec3 _pos, const double xRot, const double yRot);

	void setPosition(glm::dvec3 _pos);
	void setPositionRotation(glm::dvec3 _pos, const double xRot, const double yRot);
	void setRotation(const double xRot, const double yRot);
	void moveAbsolute(glm::dvec3 _offset);
	void displaceForward(const double distance);
	void displaceSideways(const double distance);
	void displaceVertical(const double distance);
	void rotate(const double xRot, const double yRot);

	glm::dvec3 pos;
	glm::dvec3 displacement;
	double xRotation;
	double yRotation;

private:
	void wrapCoordinates();
};
