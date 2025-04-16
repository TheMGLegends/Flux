#include "Sphere.h"

#include "Engine/Entities/Components/Visualizer.h"

namespace Flux
{
	Sphere::Sphere()
	{
		if (std::shared_ptr<Visualizer> visualizer = AddComponent<Visualizer>(this).lock())
		{
			visualizer->SetModel("Sphere");
		}
	}

	Sphere::~Sphere() = default;
}
