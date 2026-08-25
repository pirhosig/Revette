#include "GenerationUnitPos2D.h"







namespace {

// TODO: unfuck this
constexpr i32 WORLD_RADIUS_GENERATION_UNIT = 16U;
constexpr i32 WORLD_DIAMETER_GENERATION_UNIT = 32U;



// Managed to unfuck this at least
inline i32 generationUnitWrapCoordinate(i32 x) {
    return (x << (31 - GENERATION_UNIT_WIDTH_C_LOG) >> (31 - GENERATION_UNIT_WIDTH_C_LOG));
}

}



GenerationUnitPos2D::OffsetType::OffsetType(i32 _x, i32 _z) :
    x{generationUnitWrapCoordinate(_x)},
    z{generationUnitWrapCoordinate(_z)}
{}



i32 GenerationUnitPos2D::OffsetType::getX() const { return x; }
i32 GenerationUnitPos2D::OffsetType::getZ() const { return z; }



GenerationUnitPos2D::GenerationUnitPos2D(i32 _x, i32 _z) :
    x{generationUnitWrapCoordinate(_x)},
    z{generationUnitWrapCoordinate(_z)}
{}



GenerationUnitPos2D::GenerationUnitPos2D(ChunkPos2D pos) :
    x{pos.getX() >> GENERATION_UNIT_WIDTH_C_LOG},
    z{pos.getZ() >> GENERATION_UNIT_WIDTH_C_LOG}
{}



i32 GenerationUnitPos2D::getX() const { return x; }
i32 GenerationUnitPos2D::getZ() const { return z; }



i32 GenerationUnitPos2D::getManhattanDistance(const GenerationUnitPos2D& other) const {
    return std::abs(x - other.x) + std::abs(z - other.z);
}



GenerationUnitPos2D::OffsetType GenerationUnitPos2D::getOffsetTo(const GenerationUnitPos2D& other) const {
    return OffsetType(other.x - x, other.z - z);
}



GenerationUnitPos2D GenerationUnitPos2D::calculateOffsetBy(i32 xDistance, i32 zDistance) const {
    return GenerationUnitPos2D(x + xDistance, z + zDistance);
}
