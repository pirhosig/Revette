#pragma once
#include <cstdint>
#include <glad/glad.h>



#pragma pack(push, 4)
struct Character
{
	uint8_t x;
	uint8_t y;
	uint8_t u;
	uint8_t v;
	uint8_t t;
};
#pragma pack(pop)



class MeshText
{
public:
	MeshText();
	MeshText(const MeshText&) = delete;

	void update(const char* text, int length);
	void draw() const;

private:
	GLuint VAO;
	GLuint VBO;
	GLuint EBO;

	GLuint triangleCount;
};
