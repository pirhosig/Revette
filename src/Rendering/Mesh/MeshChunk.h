#pragma once
#include <memory>

#include <glad/glad.h>
#include <glm/mat4x4.hpp>

#include "../../World/ChunkPos.h"
#include "MeshDataChunk.h"

class ShaderProgram;




class MeshChunk
{
public:
	MeshChunk(std::unique_ptr<MeshDataChunk> meshData);
	MeshChunk(const MeshChunk&) = delete;
	~MeshChunk();

	void drawOpaque(const ShaderProgram& shader, const glm::mat4& transformMatrix, ChunkPos playerPosition) const;
	void drawTransparent(const ShaderProgram& shader, const glm::mat4& transformMatrix, ChunkPos playerPosition) const;
	ChunkPos getPosition() const { return position; }

private:
	ChunkPos position;

	GLuint VAO;
	GLuint VBO;
	GLuint EBO;

	GLuint triangleCountOpaque;
	GLuint triangleCountTransparent;
};