#pragma once
#include <memory>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "../../World/ChunkPos.h"
#include "../ShaderProgram.h"
#include "MeshDataChunk.h"



class MeshChunk
{
public:
	MeshChunk(std::unique_ptr<MeshDataChunk> meshData);
	MeshChunk(const MeshChunk&) = delete;
	~MeshChunk();

	void draw(const ShaderProgram& shader, const glm::mat4& transformMatrix) const;

private:
	ChunkPos position;

	GLuint VAO;
	GLuint VBO;
	GLuint EBO;

	GLuint triangleCount;
};