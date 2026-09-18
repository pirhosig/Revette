#include "MeshBuilder.h"



void MeshBuilder::addChunkBlockData(const Chunk& chunk) {
    auto blockData = chunk.clone_block_container();
    globalState.incomingBlockData.push(std::move(blockData));
}



void MeshBuilder::run() {
    // Need to wait until chunks become available
    
}
