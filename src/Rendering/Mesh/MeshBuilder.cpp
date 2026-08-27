#include "MeshBuilder.h"



void MeshBuilder::addChunkBlockData(const Chunk& chunk) {
    std::scoped_lock lock(globalState.mutexForIncomingBlockData);

    
}
