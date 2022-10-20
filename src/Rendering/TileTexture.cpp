#include "TileTexture.h"
#include <memory>
#include <vector>
#include <stdexcept>

#include <glad/glad.h>
#include <stb_image.h>

#include "../GlobalLog.h"


constexpr long long TEXTURE_PIXEL_MEMORY_SIZE = 4;
//constexpr long long TEXTURE_MEMORY_SIZE = TEXTURE_SIZE * TEXTURE_SIZE * TEXTURE_PIXEL_MEMORY_SIZE;
constexpr GLenum PIXEL_FORMAT = GL_RGBA;


TileTexture::TileTexture(const char* textureFilePath, std::size_t textureWidth, std::size_t textureHeight, bool mipmapsEnabled)
{
	textureID = 0;

	int imageWidth{}, imageHeight{}, imageColourChannelCount{};
	std::unique_ptr<unsigned char[]> imageDataArray(stbi_load(textureFilePath, &imageWidth, &imageHeight, &imageColourChannelCount, 0));
	if (!imageDataArray)
	{
		GlobalLog.Write("Failed to load tile texture.");
		throw std::runtime_error("Failed to load tile texture");
	}

	const std::size_t atlasWidth = imageWidth / textureWidth;
	const std::size_t atlasHeight = imageHeight / textureHeight;
	const std::size_t atlasTextureCount = atlasWidth * atlasHeight;
	const std::size_t textureMemorySize = textureWidth * textureHeight * TEXTURE_PIXEL_MEMORY_SIZE;
	const GLenum pixelFormat = GL_RGBA; //((imageColourChannelCount == 3) ? GL_RGB : GL_RGBA);

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D_ARRAY, textureID);

	// Set magnification settings
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	if (mipmapsEnabled)
	{
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	// Allocate memory for texture array
	// [std::size_t] is cast to [int], due to the implementation
	glTexImage3D(
		GL_TEXTURE_2D_ARRAY,
		0,
		PIXEL_FORMAT,
		static_cast<GLsizei>(textureWidth),
		static_cast<GLsizei>(textureHeight),
		static_cast<GLsizei>(atlasTextureCount),
		0,
		pixelFormat,
		GL_UNSIGNED_BYTE,
		NULL
	);

	// Create a byte array to hold the specific texture which is read from the main atlas image
	std::vector<unsigned char> textureDataArray(textureMemorySize);

	for (std::size_t j = 0; j < atlasHeight; ++j)
	{
		for (std::size_t i = 0; i < atlasWidth; ++i)
		{
			// clear the data array
			textureDataArray.clear();

			// Copy the texture line by line from the atlas to the array
			std::size_t startLine = j * textureHeight;
			for (int lineCount = 0; lineCount < textureHeight; ++lineCount)
			{
				std::size_t linePosition = startLine + lineCount;
				unsigned char* lineBegin = imageDataArray.get() + ((linePosition * imageWidth) + (i * textureWidth)) * TEXTURE_PIXEL_MEMORY_SIZE;
				unsigned char* lineEnd = lineBegin + textureWidth * TEXTURE_PIXEL_MEMORY_SIZE;
				unsigned char* textureDataPosition = textureDataArray.data() + textureWidth * lineCount * TEXTURE_PIXEL_MEMORY_SIZE;
				std::copy(lineBegin, lineEnd, textureDataPosition);
			}

			// Add the texture to the texture array
			// Yet again, it pains me to cast an std::size_t to an int, but this is what the function wants
			glTexSubImage3D(
				GL_TEXTURE_2D_ARRAY,
				0,
				0,
				0,
				static_cast<GLint>(j * atlasWidth + i),
				static_cast<GLsizei>(textureWidth),
				static_cast<GLsizei>(textureHeight),
				1,
				PIXEL_FORMAT,
				GL_UNSIGNED_BYTE,
				textureDataArray.data()
			);
		}
	}

	if (mipmapsEnabled) glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
}



TileTexture::~TileTexture()
{
	if (textureID) glDeleteTextures(1, &textureID);
}



void TileTexture::bindTexture() const
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, textureID);
}