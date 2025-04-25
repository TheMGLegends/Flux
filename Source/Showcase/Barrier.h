#pragma once

#include "Engine/Entities/GameObjects/GameObject.h"

namespace Flux
{
	class Barrier : public GameObject
	{
		REFLECT(Barrier);

	public:
		Barrier();
		~Barrier() override;
	};
}

