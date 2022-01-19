#include "MeshChunk.h"
#include <vector>

#include <glm/gtc/matrix_transform.hpp>

#include "../../World/Block.h"
#include "../../World/World.h"

using VERTEX_COORD_TYPE = uint16_t;
using VERTEX_TEX_LAYER_COORD_TYPE = uint16_t;
using VERTEX_TEX_COORD_TYPE = uint8_t;
using INDEX_TYPE = uint32_t;
constexpr GLenum TYPEGL_VERTEX_COORD_TYPE = GL_UNSIGNED_SHORT;
constexpr GLenum TYPEGL_VERTEX_TEX_LAYER_COORD_TYPE = GL_UNSIGNED_SHORT;
constexpr GLenum TYPEGL_VERTEX_TEX_COORD_TYPE = GL_UNSIGNED_BYTE;
constexpr GLenum TYPEGL_INDEX_TYPE = GL_UNSIGNED_INT;


#pragma pack(push, 1)
struct Vertex
{
	VERTEX_COORD_TYPE x;
	VERTEX_COORD_TYPE y;
	VERTEX_COORD_TYPE z;
	VERTEX_TEX_LAYER_COORD_TYPE t;
	VERTEX_TEX_COORD_TYPE u;
	VERTEX_TEX_COORD_TYPE v;
};
#pragma pack(pop)



MeshChunk::MeshChunk(World& world, ChunkPos chunkPos) : position(chunkPos)
{
	// Offsets for every vertex to draw a cube
	const int FACE_TABLE[6][4][3] = {
		// Up
		{{ 0, 1, 0 }, { 1, 1, 0 }, { 1, 1, 1 }, {0, 1, 1}},
		// Down
		{{ 0, 0, 0 }, { 1, 0, 0 }, { 1, 0, 1 }, {0, 0, 1}},
		// North
		{{ 1, 1, 1 }, { 1, 1, 0 }, { 1, 0, 0 }, {1, 0, 1}},
		// South
		{{ 0, 1, 1 }, { 0, 1, 0 }, { 0, 0, 0 }, {0, 0, 1}},
		// East
		{{ 0, 0, 1 }, { 1, 0, 1 }, { 1, 1, 1 }, {0, 1, 1}},
		// West
		{{ 0, 0, 0 }, { 1, 0, 0 }, { 1, 1, 0 }, {0, 1, 0}}
	};

	const int TEXTURE_COORDINATES[4][2] = {
		{ 0, 0 },
		{ 1, 0 },
		{ 1, 1 },
		{ 0, 1 }
	};

	triangleCount = 0;
	VAO = NULL;
	VBO = NULL;
	EBO = NULL;

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	GLuint buffers[2]{};
	glGenBuffers(2, &buffers[0]);

	VBO = buffers[0];
	EBO = buffers[1];

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	std::vector<Vertex> verticies;
	std::vector<INDEX_TYPE> indicies;
	int indexCounter = 0;

	for (int i = 0; i < CHUNK_SIZE; ++i)
	{
		for (int j = 0; j < CHUNK_SIZE; ++j)
		{
			for (int k = 0; k < CHUNK_SIZE; ++k)
			{
				ChunkLocalBlockPos localPos(i, j, k);
				BlockPos worldPos = localPos.asBlockPos(chunkPos);

				Block block = world.getBlock(worldPos);
				// Skip if air block
				if (block.blockType == 0) continue;

				// Loop through all block faces
				for (int l = 0; l < 6; ++l)
				{
					AxisDirection neighborDirection = static_cast<AxisDirection>(l);
					Block neighbor = world.getBlock(worldPos.direction(neighborDirection));

					// Only add face if the adjacent block is transparent
					if (neighbor.blockType != 0) continue;

					triangleCount += 2;

					indicies.push_back(indexCounter);
					indicies.push_back(indexCounter + 1);
					indicies.push_back(indexCounter + 2);
					indicies.push_back(indexCounter + 2);
					indicies.push_back(indexCounter + 3);
					indicies.push_back(indexCounter);
					indexCounter += 4;

					// Loop through each vertex of the face
					for (int v = 0; v < 4; ++v)
					{
						Vertex vertex{
							static_cast<uint16_t>(localPos.x + FACE_TABLE[l][v][0]),
							static_cast<uint16_t>(localPos.y + FACE_TABLE[l][v][1]),
							static_cast<uint16_t>(localPos.z + FACE_TABLE[l][v][2]),
							static_cast<uint8_t>(block.blockType - 1),
							static_cast<uint8_t>(TEXTURE_COORDINATES[v][0]),
							static_cast<uint8_t>(TEXTURE_COORDINATES[v][1])
						};
						verticies.push_back(vertex);
					}
				}
			}
		}
	}

	glVertexAttribPointer(0, 3, TYPEGL_VERTEX_COORD_TYPE, GL_FALSE, sizeof(Vertex), (void*)(0));
	glVertexAttribIPointer(1, 1, TYPEGL_VERTEX_TEX_LAYER_COORD_TYPE, sizeof(Vertex), (void*)(sizeof(VERTEX_COORD_TYPE) * 3));
	glVertexAttribPointer(
		2,
		2,
		TYPEGL_VERTEX_TEX_COORD_TYPE,
		GL_FALSE,
		sizeof(Vertex),
		(void*)(sizeof(VERTEX_COORD_TYPE) * 3 + sizeof(VERTEX_TEX_LAYER_COORD_TYPE))
	);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)* verticies.size(), verticies.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(INDEX_TYPE)* indicies.size(), indicies.data(), GL_DYNAMIC_DRAW);

	glBindVertexArray(0);
}



MeshChunk::~MeshChunk()
{
	GLuint buffers[2] = { VBO, EBO };
	glDeleteBuffers(2, &buffers[0]);
	glDeleteVertexArrays(1, &VAO);
}



// Draws the mesh, the shader program should already be active when this function is called
void MeshChunk::draw(std::unique_ptr<ShaderProgram>& shader, glm::mat4& transformMatrix) const
{
	BlockPos worldPosition = ChunkLocalBlockPos(0, 0, 0).asBlockPos(position);
	glm::vec3 chunkVector(worldPosition.x, worldPosition.y, worldPosition.z);
	glm::mat4 model = glm::translate(glm::mat4(1.0f), chunkVector);
	glm::mat4 modelViewProjection = transformMatrix * model;
	shader->setMat4("TransformMatrix", modelViewProjection);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, triangleCount * 3, TYPEGL_INDEX_TYPE, 0);

	glBindVertexArray(0);
}
