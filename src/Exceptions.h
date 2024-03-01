#pragma once
#include <stdexcept>
#include <string>



namespace EXCEPTION_WORLD
{
	class BlockIndexOutOfRange : public std::runtime_error
	{
	public:
		BlockIndexOutOfRange(const char* message) : std::runtime_error(message) {};
		BlockIndexOutOfRange(const std::string& message) : std::runtime_error(message.c_str()) {};
	};

	
	class ChunkNonExistence : public std::runtime_error
	{
	public:
		ChunkNonExistence(const char* message) : std::runtime_error(message) {};
		ChunkNonExistence(const std::string& message) : std::runtime_error(message.c_str()) {};
	};

	class StructureNonExistence : public std::runtime_error
	{
	public:
		StructureNonExistence(const char* message) : std::runtime_error(message) {};
		StructureNonExistence(const std::string& message) : std::runtime_error(message.c_str()) {};
	};


	class ChunkRegeneration : public std::runtime_error
	{
	public:
		ChunkRegeneration(const char* message) : std::runtime_error(message) {};
		ChunkRegeneration(const std::string& message) : std::runtime_error(message.c_str()) {};
	};
}