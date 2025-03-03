#pragma once

#include "Core/EventSystem/EventDispatcher.h"

namespace Flux
{
	class Runtime
	{
	public:
		Runtime() = default;
		virtual ~Runtime() = 0 {}

		/// @brief Used for initializing systems before the main initialisation
		virtual bool PreInitialise() = 0;
		/// @brief Used for initializing data that uses the systems
		virtual bool Initialise() = 0;
		virtual void Update(float deltaTime) {}
	};
}

