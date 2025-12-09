#include "SharedGameRendererState.h"



SharedGameRendererState::SharedGameRendererState() :
    chunkMeshQueue{std::make_shared<decltype(chunkMeshQueue)::element_type>()},
    chunkMeshQueueDeletion{std::make_shared<decltype(chunkMeshQueueDeletion)::element_type>()}
{}
