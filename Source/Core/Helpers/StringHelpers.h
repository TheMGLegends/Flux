#pragma once

#include <string>

namespace Flux::StringHelpers
{
	/// @brief Converts a new copy of the string to lowercase
	std::string ToLower(const std::string& string);

	/// @brief Converts the original passed string to lowercase
	void ToLower(std::string& string);

	/// @brief Converts a new copy of the string to uppercase
    std::string ToUpper(const std::string& string);

	/// @brief Converts the original passed string to uppercase
	void ToUpper(std::string& string);
}

