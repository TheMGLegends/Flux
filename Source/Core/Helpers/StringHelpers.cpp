#include "StringHelpers.h"

#include <algorithm>

namespace Flux::StringHelpers
{
	std::string ToLower(const std::string& string)
	{
		std::string lowerString = string;
		std::transform(lowerString.begin(), lowerString.end(), lowerString.begin(), ::tolower);
		return lowerString;
	}

	void ToLower(std::string& string)
	{
		std::transform(string.begin(), string.end(), string.begin(), ::tolower);
	}

	std::string ToUpper(const std::string& string)
	{
		std::string upperString = string;
		std::transform(upperString.begin(), upperString.end(), upperString.begin(), ::toupper);
		return upperString;
	}

	void ToUpper(std::string& string)
	{
		std::transform(string.begin(), string.end(), string.begin(), ::toupper);
	}
}
