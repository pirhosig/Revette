#include "TileTexture.h"
#include <array>
#include <iostream>
#include <memory>
#include <stdexcept>

#include <glad/glad.h>
#include <stb_image.h>

constexpr long long TEXTURE_SIZE = 16;
constexpr long long TEXTURE_PIXEL_MEMORY_SIZE = 4;
constexpr long long TEXTURE_MEMORY_SIZE = TEXTURE_SIZE * TEXTURE_SIZE * TEXTURE_PIXEL_MEMORY_SIZE;
constexpr GLenum PIXEL_FORMAT = GL_RGBA;



TileTexture::TileTexture(const char* textureFilePath, const GLenum imageType)
{
	textureID = 0;

	int imageWidth, imageHeight, imageColourChannelCount;
	std::unique_ptr<unsigned char[]> imageDataArray(stbi_load(textureFilePath, &imageWidth, &imageHeight, &imageColourChannelCount, 0));
	if (!imageDataArray)
	{
		std::cout << "Failed to load tile texture." << std::endl;
		throw std::runtime_error("Failed to load tile texture");
	}

	const int atlasWidth = imageWidth / TEXTURE_SIZE;
	const int atlasHeight = imageHeight / TEXTURE_SIZE;
	const int atlasTextureCount = atlasWidth * atlasHeight;

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D_ARRAY, textureID);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, PIXEL_FORMAT, TEXTURE_SIZE, TEXTURE_SIZE, atlasTextureCount, 0, imageType, GL_UNSIGNED_BYTE, NULL);

	for (int j = 0; j < atlasHeight; ++j)
	{
		for (int i = 0; i < atlasWidth; ++i)
		{
			// Create a byte array to hold the specific texture which is read from the main atlas image
			std::array<unsigned char, TEXTURE_MEMORY_SIZE> textureDataArray;

			// Copy the texture line by line from the atlas to the array
			int startLine = j * TEXTURE_SIZE;
			for (int lineCount = 0; lineCount < TEXTURE_SIZE; ++lineCount)
			{
				int linePosition = startLine + lineCount;
				unsigned char* lineBegin = imageDataArray.get() + ((static_cast<long long>(linePosition) * imageWidth) + (i * TEXTURE_SIZE)) * TEXTURE_PIXEL_MEMORY_SIZE;
				unsigned char* lineEnd = lineBegin + TEXTURE_SIZE * TEXTURE_PIXEL_MEMORY_SIZE;
				unsigned char* textureDataPosition = textureDataArray.data() + TEXTURE_SIZE * lineCount * TEXTURE_PIXEL_MEMORY_SIZE;
				std::copy(lineBegin, lineEnd, textureDataPosition);
			}

			// Add the texture to the texture array
			glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, (j * atlasWidth + i), TEXTURE_SIZE, TEXTURE_SIZE, 1, PIXEL_FORMAT, GL_UNSIGNED_BYTE, textureDataArray.data());
		}
	}
}