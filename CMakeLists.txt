cmake_minimum_required(VERSION 3.12)

project(Revette)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(Revette 
    src/GlobalLog.cpp
    src/Logger.cpp
    src/LoopGame.cpp
    src/LoopTimer.cpp
    src/main.cpp
    src/MainLoop.cpp

    src/Rendering/Renderer.cpp
    src/Rendering/ShaderProgram.cpp
    src/Rendering/TileTexture.cpp

    src/Rendering/Mesh/MeshChunk.cpp
    src/Rendering/Mesh/MeshDataChunk.cpp
    src/Rendering/Mesh/MeshText.cpp

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

    deps/glad/src/glad.c
    deps/lodepng/lodepng.cpp
)

add_subdirectory(deps/FastNoise2)

target_compile_options(Revette PUBLIC -O3 -Wall -Wextra)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

target_link_libraries(Revette PRIVATE glfw FastNoise)

target_include_directories(Revette PRIVATE ${GLFW_INCLUDE_PATH} ${GLEW_INCLUDE_PATH} ${GLM_INCLUDE_PATH} deps/glad/include deps/lodepng/)




