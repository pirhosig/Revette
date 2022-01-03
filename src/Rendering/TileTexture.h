#pragma once
#include <glad/glad.h>



class TileTexture
{
public:
	TileTexture(const char* textureFilePath, const GLenum imageType);
	void bindTexture();

private:
	GLuint textureID;
};