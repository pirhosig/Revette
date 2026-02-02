cmake_minimum_required(VERSION 3.20)

project(Revette)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(Revette 
    src/main.cpp
    
    src/Application.cpp
    src/GlobalLog.cpp
    src/Logger.cpp
    src/LoopGame.cpp
    src/LoopTimer.cpp
    src/Window.cpp

    src/Rendering/Buffer.cpp
    src/Rendering/ChunkRenderer.cpp
    src/Rendering/Fence.cpp
    src/Rendering/FrameRenderer.cpp
    src/Rendering/GuiRenderer.cpp
    src/Rendering/LinearBufferSuballocator.cpp
    src/Rendering/Renderer.cpp
    src/Rendering/RenderResources.cpp
    src/Rendering/RenderTarget.cpp
    src/Rendering/SingleCommandBuffer.cpp
    src/Rendering/Vulkan_Utils.cpp
    src/Rendering/VulkanContext.cpp
    src/Rendering/Mesh/MeshChunk.cpp

    src/Threading/SharedGameRendererState.cpp

    src/World/Block.cpp
    src/World/BlockContainer.cpp
    src/World/Chunk.cpp
    src/World/ChunkPos.cpp
    src/World/ChunkStatusMap.cpp
    src/World/StatusChunk.cpp
    src/World/World.cpp

    src/World/Entities/Entity.cpp
    src/World/Entities/EntityPosition.cpp

    src/World/Generation/BiomeMap.cpp
    src/World/Generation/ChunkPRNG.cpp
    src/World/Generation/HeightMap.cpp
    src/World/Generation/NoiseSource.cpp

    src/World/Generation/Structures/Structure.cpp
    src/World/Generation/Structures/StructureBoundingBox.cpp
    src/World/Generation/Structures/StructurePlants.cpp
    src/World/Generation/Structures/StructuresRuins.cpp

    deps/lodepng/lodepng.cpp
    deps/VMA/vma_implementation.cpp
)

set_source_files_properties(
    deps/lodepng/lodepng.cpp
    PROPERTIES
    COMPILE_FLAGS "-w"
)

set_property(TARGET Revette PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)

add_subdirectory(deps/FastNoise2)

target_compile_options(Revette PUBLIC -O3 -march=native -Werror -Wall -Wextra -Wpedantic -Wfloat-conversion)
target_link_options(Revette PUBLIC -flto=auto)

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

target_link_libraries(
    Revette PRIVATE
    vulkan
    glfw
    FastNoise
    atomic
)

target_include_directories(Revette PRIVATE
    ${GLFW_INCLUDE_PATH}
    ${GLM_INCLUDE_PATH}
    deps/
)




