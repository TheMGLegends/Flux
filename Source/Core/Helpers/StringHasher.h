#pragma once

#include <string>

struct StringHasher
{
	using is_transparent = void;

	size_t operator()(const std::string_view str) const
	{
		return std::hash<std::string_view>{}(str);
	}
};

