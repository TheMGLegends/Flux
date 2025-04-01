#pragma once

#include "../GameObject.h"

namespace Flux
{
	class Cube : public GameObject
	{
		REFLECT(Cube);

	public:
		Cube();
		virtual ~Cube() override;
	};
}

