#pragma once
#include <stdexcept>
#include <string>



namespace EXCEPTION_WORLD
{
	class BlockIndexOutOfRange : public std::exception
	{
	public:
		BlockIndexOutOfRange(const char* message) : std::exception(message) {};
		BlockIndexOutOfRange(const std::string& message) : std::exception(message.c_str()) {};
	};

	
	class ChunkNonExistence : public std::exception
	{
	public:
		ChunkNonExistence(const char* message) : std::exception(message) {};
		ChunkNonExistence(const std::string& message) : std::exception(message.c_str()) {};
	};

	class StructureNonExistence : public std::exception
	{
	public:
		StructureNonExistence(const char* message) : std::exception(message) {};
		StructureNonExistence(const std::string& message) : std::exception(message.c_str()) {};
	};


	class ChunkRegeneration : public std::exception
	{
	public:
		ChunkRegeneration(const char* message) : std::exception(message) {};
		ChunkRegeneration(const std::string& message) : std::exception(message.c_str()) {};
	};
}