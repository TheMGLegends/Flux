#pragma once

#include "../GameObject.h"

namespace Flux
{
	class Sphere : public GameObject
	{
		REFLECT(Sphere);

	public:
		Sphere();
		~Sphere() override;
	};
}

