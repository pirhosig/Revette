#pragma once
#include <glad/glad.h>



class Texture
{
public:
	void bindTexture();
	bool loadTexture(const char* textureFilePath, GLenum imageType);

private:
	GLuint textureID = 0;
};