#pragma once

#include "Flux/Core.h"

#include <sfml/System/Vector2.hpp>
#include <string>

namespace Flux
{
	/// <summary>
	/// Wrapper class for Vector2 provided by SFML
	/// </summary>
	template <typename T>
	class Vector2 : public sf::Vector2<T>
	{
	public:
		Vector2() : sf::Vector2<T>() {}
		Vector2(const sf::Vector2<T>& vector) : sf::Vector2<T>(vector) {}

		std::string ToString() const
		{
			return std::format("({0}, {1})", this->x, this->y);
		}

		Vector2<T>& operator=(const sf::Vector2<T>& other)
		{
			if (this == &other) { return *this; }

			this->x = other.x;
			this->y = other.y;
			return *this;
		}
	};

	using Vector2I = Vector2<int>;
	using Vector2U = Vector2<unsigned int>;
	using Vector2F = Vector2<float>;
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const Flux::Vector2<T>& vector)
{
	os << vector.ToString();
	return os;
}

// INFO: Specialization for Logging Vector2 Wrapper
template<typename T, typename CharT>
struct std::formatter<Flux::Vector2<T>, CharT> : std::formatter<std::string>
{
	template <typename FormatContext>
	FormatContext::iterator format(const Flux::Vector2<T>& vector, FormatContext& ctx) const
	{
		return std::formatter<std::string>::format(vector.ToString(), ctx);
	}
};