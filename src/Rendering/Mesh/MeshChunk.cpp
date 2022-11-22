#include "MeshChunk.h"
#include <vector>
#include <numbers>

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
		GL_TRUE,
		sizeof(Vertex),
		(void*)(8)
	);
	glVertexAttribPointer(3, 1, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void*)(10));
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



bool pointFallsWithinFOV(glm::vec3 point, glm::mat4& transformMatrix)
{
	glm::vec4 clipCoordinates = transformMatrix * glm::vec4(point, 1.0);
	return (std::abs(clipCoordinates.x / clipCoordinates.w) <= 1.0 && std::abs(clipCoordinates.y / clipCoordinates.w) <= 1.0);
}



bool withinFOV(glm::mat4& transformMatrix)
{
	constexpr double CS = static_cast<double>(CHUNK_SIZE) + 1.0;
	// Check if any of the eight corners fall within the FOV
	return (
		pointFallsWithinFOV(glm::vec3(0.0, 0.0, 0.0), transformMatrix) ||
		pointFallsWithinFOV(glm::vec3(0.0, 0.0,  CS), transformMatrix) ||
		pointFallsWithinFOV(glm::vec3(0.0,  CS, 0.0), transformMatrix) ||
		pointFallsWithinFOV(glm::vec3(0.0,  CS,  CS), transformMatrix) ||
		pointFallsWithinFOV(glm::vec3( CS, 0.0, 0.0), transformMatrix) ||
		pointFallsWithinFOV(glm::vec3( CS, 0.0,  CS), transformMatrix) ||
		pointFallsWithinFOV(glm::vec3( CS,  CS, 0.0), transformMatrix) ||
		pointFallsWithinFOV(glm::vec3( CS,  CS,  CS), transformMatrix)
	);
}



// Draws the mesh, the shader program should already be active when this function is called
void MeshChunk::draw(const ShaderProgram& shader, const glm::mat4& transformMatrix, ChunkPos playerPosition) const
{
	ChunkOffset offset = playerPosition.offset(position);
	glm::vec3 chunkVector = glm::vec3(offset.x * CHUNK_SIZE, offset.y * CHUNK_SIZE, offset.z * CHUNK_SIZE);
	glm::mat4 modelViewProjection = transformMatrix * glm::translate(glm::mat4(1.0f), chunkVector);

	// Return if the chunk is outside the FOV, and the chunk is not very close to the player
	if ((offset.x * offset.x + offset.y * offset.y + offset.z * offset.z > 5) && !withinFOV(modelViewProjection)) return;

	shader.setMat4("transform", modelViewProjection);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, triangleCount * 3, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
}


