#pragma once

// TODO: TEST CLASS

#include "GameObject.h"

namespace Flux
{
	class Player : public GameObject
	{
		REFLECT(Player)

	public:
		Player();
		virtual ~Player() override;
	};
}

