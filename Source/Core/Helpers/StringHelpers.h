#pragma once

#include <algorithm>
#include <string>

namespace Flux::StringHelpers
{
	/// @brief Converts a new copy of the string to lowercase
	inline std::string ToLower(const std::string& string)
	{
		std::string lowerString = string;
		std::transform(lowerString.begin(), lowerString.end(), lowerString.begin(), ::tolower);
		return lowerString;
	}

	/// @brief Converts the original passed string to lowercase
	inline void ToLower(std::string& string)
	{
		std::transform(string.begin(), string.end(), string.begin(), ::tolower);
	}

	/// @brief Converts a new copy of the string to uppercase
	inline std::string ToUpper(const std::string& string)
	{
		std::string upperString = string;
		std::transform(upperString.begin(), upperString.end(), upperString.begin(), ::toupper);
		return upperString;
	}

	/// @brief Converts the original passed string to uppercase
	inline void ToUpper(std::string& string)
	{
		std::transform(string.begin(), string.end(), string.begin(), ::toupper);
	}
}