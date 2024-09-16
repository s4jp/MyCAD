#pragma once

#include <stdexcept>

namespace MG1
{
	struct SerializerException : public std::runtime_error
	{
		SerializerException(const char* message) : std::runtime_error(message) { }
	};
}
