#pragma once
#include <memory>
#include <mutex>
#include <vector>

#include "Core/RevetteCore.h"
#include "World/BlockContainer.h"
#include "World/Chunk.h"



class MeshBuilder {
    struct GlobalStateType {
		std::mutex mutexForIncomingBlockData;
		std::vector<BlockContainer> incomingBlockData;

		GlobalStateType();
	};
	static GlobalStateType globalState;



public:
    static void addChunkBlockData(const Chunk& chunk);

    

public:
    void run();

};
