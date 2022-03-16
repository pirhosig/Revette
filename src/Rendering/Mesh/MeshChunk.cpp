#include "MeshChunk.h"
#include <vector>

#include <glm/gtc/matrix_transform.hpp>



// Creates a chunk mesh using the passed data object, the data members of "meshData" are moved into this instance
MeshChunk::MeshChunk(std::unique_ptr<MeshDataChunk> meshData) : triangleCount(meshData->triangleCount), position(meshData->position)
{
	VAO = 0;
	VBO = 0;
	EBO = 0;

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	GLuint buffers[2]{};
	glGenBuffers(2, &buffers[0]);

	VBO = buffers[0];
	EBO = buffers[1];

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	glVertexAttribPointer(0, 3, GL_UNSIGNED_SHORT, GL_FALSE, sizeof(Vertex), (void*)(0));
	glVertexAttribPointer(1, 1, GL_UNSIGNED_SHORT, GL_FALSE, sizeof(Vertex), (void*)(6));
	glVertexAttribPointer(
		2,
		2,
		GL_UNSIGNED_BYTE,
		GL_FALSE,
		sizeof(Vertex),
		(void*)(8)
	);
	glVertexAttribPointer(3, 1, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(Vertex), (void*)(10));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)* meshData->verticies.size(), meshData->verticies.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * meshData->indicies.size(), meshData->indicies.data(), GL_DYNAMIC_DRAW);

	glBindVertexArray(0);
}



MeshChunk::~MeshChunk()
{
	GLuint buffers[2] = { VBO, EBO };
	glDeleteBuffers(2, &buffers[0]);
	glDeleteVertexArrays(1, &VAO);
}



// Draws the mesh, the shader program should already be active when this function is called
void MeshChunk::draw(const ShaderProgram& shader, const glm::mat4& transformMatrix) const
{
	BlockPos worldPosition = ChunkLocalBlockPos(0, 0, 0).asBlockPos(position);
	glm::vec3 chunkVector(worldPosition.x, worldPosition.y, worldPosition.z);
	glm::mat4 modelViewProjection = transformMatrix * glm::translate(glm::mat4(1.0f), chunkVector);
	shader.setMat4("transform", modelViewProjection);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, triangleCount * 3, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
}
