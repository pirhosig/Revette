#pragma once
#include <glad/glad.h>



class TileTexture
{
public:
	TileTexture(const char* textureFilePath);
	void bindTexture() const;

private:
	GLuint textureID;
};