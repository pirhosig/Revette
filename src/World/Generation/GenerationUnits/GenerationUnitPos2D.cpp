#include "GenerationUnitPos2D.h"







namespace {

// TODO: unfuck this
constexpr int32_t WORLD_RADIUS_GENERATION_UNIT = 16U;
constexpr int32_t WORLD_DIAMETER_GENERATION_UNIT = 32U;

inline int32_t generationUnitWrapCoordinate(int32_t x)
{
    if (-WORLD_RADIUS_GENERATION_UNIT <= x && x < WORLD_RADIUS_GENERATION_UNIT) return x;
    x %= WORLD_DIAMETER_GENERATION_UNIT;
    if (x < -WORLD_RADIUS_GENERATION_UNIT)      x += WORLD_DIAMETER_GENERATION_UNIT;
    else if (WORLD_RADIUS_GENERATION_UNIT <= x) x -= WORLD_DIAMETER_GENERATION_UNIT;
    return x;
}

}



GenerationUnitPos2D::GenerationUnitPos2D(int32_t _x, int32_t _z) :
    x{generationUnitWrapCoordinate(_x)},
    z{generationUnitWrapCoordinate(_z)}
{}



GenerationUnitPos2D::GenerationUnitPos2D(ChunkPos2D pos) :
    x{pos.getX() >> GENERATION_UNIT_WIDTH_C_LOG},
    z{pos.getZ() >> GENERATION_UNIT_WIDTH_C_LOG}
{}



int32_t GenerationUnitPos2D::getX() const { return x; }
int32_t GenerationUnitPos2D::getZ() const { return z; }



GenerationUnitPos2D GenerationUnitPos2D::offsetBy(int32_t xOffset, int32_t zOffset) const {
    return GenerationUnitPos2D(x + xOffset, z + zOffset);
}
