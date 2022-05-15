#include "Structure.h"
#include "../../World.h"



Structure::Structure(unsigned long long _age, BlockPos _lowerCorner, BlockPos _upperCorner, BlockPos _centre) :
	age(_age),
	boundingBox(_lowerCorner, _upperCorner),
	centre(_centre)
{}



BlockPos Structure::getCentre() const
{
	return centre;
}



StructureBoundingBox Structure::getBoundingBox() const
{
	return boundingBox;
}


