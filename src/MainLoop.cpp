#include "MainLoop.h"
#include "LoopGame.h"

#include "GlobalLog.h"



void MainLoop::run()
{
	auto chunkMeshQueue = std::make_shared<ThreadPointerQueue<MeshChunk::Data>>();
	auto chunkMeshQueueDeletion = std::make_shared<ThreadQueue<ChunkPos>>();
	LoopGame loop(window.get(), chunkMeshQueue, chunkMeshQueueDeletion);
	loop.run();
}

