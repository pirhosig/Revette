#pragma once
#include <memory>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "../../World/ChunkPos.h"
#include "../ShaderProgram.h"
class World;




class MeshChunk
{
public:
	MeshChunk(World& world, ChunkPos chunkPos);
	~MeshChunk();

	void draw(std::unique_ptr<ShaderProgram>& shader, glm::mat4& transformMatrix) const;

private:
	ChunkPos position;

	GLuint VAO;
	GLuint VBO;
	GLuint EBO;

	GLuint triangleCount;
};