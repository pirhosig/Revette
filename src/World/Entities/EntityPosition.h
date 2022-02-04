#pragma once



class EntityPosition
{
public:
	EntityPosition();
	EntityPosition(const double x, const double y, const double z);
	EntityPosition(const double x, const double y, const double z, const double xRot, const double yRot);

	void setPosition(const double x, const double y, const double z);
	void setPositionRotation(const double x, const double y, const double z, const double xRot, const double yRot);
	void setRotation(const double xRot, const double yRot);
	void moveAbsolute(const double x, const double y, const double z);
	void moveForward(const double distance);
	void moveSideways(const double distance);
	void moveVertical(const double distance);

	double X;
	double Y;
	double Z;
	double xRotation;
	double yRotation;
};
