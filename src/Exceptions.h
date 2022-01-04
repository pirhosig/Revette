#pragma once
#include <stdexcept>



namespace EXCEPTION_WORLD
{
	class BlockIndexOutOfRange : public std::exception
	{
	public:
		BlockIndexOutOfRange(const char* message) : std::exception(message) {};
	};
}