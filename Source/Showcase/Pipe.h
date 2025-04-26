#pragma once

#include "Engine/Entities/GameObjects/GameObject.h"

namespace Flux
{
	class Pipe : public GameObject
	{
		REFLECT(Pipe);

	public:
		Pipe();
		~Pipe() override;
	};
}

