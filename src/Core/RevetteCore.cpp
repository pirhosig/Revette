#include "RevetteCore.h"

#include <thread>



/*
This file contains checks for very elementary assumptions in the codebase.
*/



static_assert(
    CACHE_ALIGNMENT == std::hardware_destructive_interference_size,
    "This cache configuration is not supported."
);

static_assert(
    std::endian::native == std::endian::little,
    "Only little endian systems are supported."
);

