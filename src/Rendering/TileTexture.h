#pragma once
#include <cstddef>
#include <glad/glad.h>



class TileTexture
{
public:
	TileTexture(const char* textureFilePath, std::size_t textureWidth, std::size_t textureHeight, bool mipmapsEnabled);
	TileTexture(const TileTexture&) = delete;
	~TileTexture();
	void bindTexture() const;

private:
	GLuint textureID;
};