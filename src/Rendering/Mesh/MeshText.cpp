#include "MeshText.h"
#include <cstddef>
#include <vector>



MeshText::MeshText()
{
	triangleCount = 0;
	VAO = 0;
	VBO = 0;
	EBO = 0;

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	{
		GLuint buffers[2]{};
		glGenBuffers(2, &buffers[0]);

		VBO = buffers[0];
		EBO = buffers[1];
	}

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	glVertexAttribPointer(0, 2, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(Character), (void*)(0));
	glVertexAttribPointer(1, 3, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(Character), (void*)(offsetof(Character, u)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}



void MeshText::update(const char* text, int length)
{
	glBindVertexArray(VAO);

	triangleCount = 0;

	std::vector<Character> verticies;
	std::vector<uint16_t> indicies;
	uint16_t indexCounter = 0;

	for (int i = 0; i < length; ++i)
	{
		if (text[i] == ' ') continue;

		int loc = 0;
		char c = text[i];
		switch (c)
		{
		case '0':
			loc = 0;
			break;
		case '1':
			loc = 1;
			break;
		case '2':
			loc = 2;
			break;
		case '3':
			loc = 3;
			break;
		case '4':
			loc = 4;
			break;
		case '5':
			loc = 5;
			break;
		case '6':
			loc = 6;
			break;
		case '7':
			loc = 7;
			break;
		case '8':
			loc = 8;
			break;
		case '9':
			loc = 9;
			break;
		case '.':
			loc = 10;
			break;
		case '-':
			loc = 11;
			break;
		default:
			loc = 0;
			break;
		}

		verticies.push_back(Character{ static_cast<uint8_t>(i    ), 0, 0, 0, static_cast<uint8_t>(loc) });
		verticies.push_back(Character{ static_cast<uint8_t>(i + 1), 0, 1, 0, static_cast<uint8_t>(loc) });
		verticies.push_back(Character{ static_cast<uint8_t>(i + 1), 1, 1, 1, static_cast<uint8_t>(loc) });
		verticies.push_back(Character{ static_cast<uint8_t>(i    ), 1, 0, 1, static_cast<uint8_t>(loc) });

		indicies.push_back(indexCounter);
		indicies.push_back(indexCounter + 2);
		indicies.push_back(indexCounter + 1);
		indicies.push_back(indexCounter + 2);
		indicies.push_back(indexCounter);
		indicies.push_back(indexCounter + 3);

		triangleCount += 2;
		indexCounter += 4;
	}

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Character) * verticies.size(), verticies.data(), GL_STREAM_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 2 * indicies.size(), indicies.data(), GL_STREAM_DRAW);

	glBindVertexArray(0);
}



void MeshText::draw() const
{
	if (!triangleCount) return;

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, triangleCount * 3, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
}
