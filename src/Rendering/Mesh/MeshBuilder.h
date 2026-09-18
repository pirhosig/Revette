#pragma once
#include <memory>
#include <mutex>
#include <vector>

#include "Settings.h"
#include "Application/GlobalApplicationState.h"
#include "Core/RevetteCore.h"
#include "Util/ConcurrentQueue.h"
#include "World/BlockContainer.h"
#include "World/Chunk.h"



class MeshBuilder {
	struct GlobalStateType {
		ConcurrentQueue<BlockContainer> incomingBlockData;

		GlobalStateType();
	};
	static GlobalStateType globalState;



public:
    static void addChunkBlockData(const Chunk& chunk);

    

private:
	GlobalApplicationState& globalApplicationState;
    const Settings& settings;



public:
	MeshBuilder();

    void run();

};
