#pragma once



namespace Math
{
	struct Vector
	{
		double X;
		double Y;
		double Z;

		Vector& operator+=(const Vector& other)
		{
			X += other.X;
			Y += other.Y;
			Z += other.Z;
			return *this;
		}

		Vector operator*(const double& scalar) const
		{
			return { X * scalar, Y * scalar, Z * scalar };
		}
	};
}