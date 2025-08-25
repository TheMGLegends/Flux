#pragma once

#include "Flux/Core.h"

#include <sfml/System/Vector2.hpp>

namespace Flux
{
	/// <summary>
	/// Wrapper class for Vector2 provided by SFML
	/// </summary>
	template <typename T>
	class Vector2 : public sf::Vector2<T>
	{
	public:
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