#include "MeshChunk.h"
#include <vector>
#include <numbers>

#include <glm/gtc/matrix_transform.hpp>

#include "../ShaderProgram.h"



// Creates a chunk mesh using the passed data object, the data members of "meshData" are moved into this instance
MeshChunk::MeshChunk(std::unique_ptr<MeshDataChunk> meshData) :
	triangleCountOpaque{ meshData->triangleCountOpaque },
	triangleCountTransparent{ meshData->triangleCountTransparent },
	position(meshData->position), VAO{}, VBO{}, EBO{}
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	GLuint buffers[2]{};
	glGenBuffers(2, &buffers[0]);

	VBO = buffers[0];
	EBO = buffers[1];

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	glVertexAttribPointer(0, 4, GL_UNSIGNED_INT_2_10_10_10_REV, GL_FALSE, sizeof(Vertex), (void*)(0));
	glVertexAttribPointer(1, 1, GL_UNSIGNED_SHORT,              GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, t)));
	glVertexAttribPointer(2, 2, GL_UNSIGNED_BYTE,                GL_TRUE, sizeof(Vertex), (void*)(offsetof(Vertex, u)));
	glVertexAttribPointer(3, 1, GL_UNSIGNED_BYTE,                GL_TRUE, sizeof(Vertex), (void*)(offsetof(Vertex, w)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);

	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * meshData->verticies.size(), meshData->verticies.data(), GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * meshData->indicies.size(), meshData->indicies.data(), GL_STATIC_DRAW);

	glBindVertexArray(NULL);
}



MeshChunk::~MeshChunk()
{
	GLuint buffers[2] = { VBO, EBO };
	glDeleteBuffers(2, &buffers[0]);
	glDeleteVertexArrays(1, &VAO);
}



// This function is pretty hacky and should probably be replaced with a better cube intersection algorithm
bool pointFallsWithinFOV(glm::vec3 point, glm::mat4& transformMatrix)
{
	glm::vec4 clipCoordinates = transformMatrix * glm::vec4(point, 1.0);
	return (std::abs(clipCoordinates.x / clipCoordinates.w) <= 1.15 && std::abs(clipCoordinates.y / clipCoordinates.w) <= 1.15);
}



bool withinFOV(glm::mat4& transformMatrix)
{
	constexpr double CS = CHUNK_SIZE_D;
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
void MeshChunk::drawOpaque(const ShaderProgram& shader, const glm::mat4& transformMatrix, ChunkPos playerPosition) const
{
	if (!triangleCountOpaque) return;
	ChunkOffset offset = playerPosition.offset(position);
	glm::mat4 modelViewProjection = transformMatrix * glm::translate(
		glm::mat4(1.0f),
		glm::vec3(offset.x * CHUNK_SIZE, offset.y * CHUNK_SIZE, offset.z * CHUNK_SIZE) * 0.5f
	);

	// Return if the chunk is outside the FOV, and the chunk is not very close to the player
	if ((offset.x * offset.x + offset.y * offset.y + offset.z * offset.z > 5) && !withinFOV(modelViewProjection)) return;

	shader.setMat4("transform", modelViewProjection);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, triangleCountOpaque * 3, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}



void MeshChunk::drawTransparent(const ShaderProgram& shader, const glm::mat4& transformMatrix, ChunkPos playerPosition) const
{
	if (!triangleCountTransparent) return;
	ChunkOffset offset = playerPosition.offset(position);
	glm::mat4 modelViewProjection = transformMatrix * glm::translate(
		glm::mat4(1.0f),
		glm::vec3(offset.x * CHUNK_SIZE, offset.y * CHUNK_SIZE, offset.z * CHUNK_SIZE) * 0.5f
	);

	// Return if the chunk is outside the FOV, and the chunk is not very close to the player
	if ((offset.x * offset.x + offset.y * offset.y + offset.z * offset.z > 5) && !withinFOV(modelViewProjection)) return;

	shader.setMat4("transform", modelViewProjection);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, triangleCountTransparent * 3, GL_UNSIGNED_INT, (void *)(static_cast<uint64_t>(triangleCountOpaque * 3 * 4)));
	glBindVertexArray(0);
}


