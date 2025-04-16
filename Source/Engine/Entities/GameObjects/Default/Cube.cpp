#include "Cube.h"

#include "Engine/Entities/Components/Visualizer.h"

namespace Flux
{
	Cube::Cube()
	{
		if (std::shared_ptr<Visualizer> visualizer = AddComponent<Visualizer>(this).lock())
		{
			visualizer->SetModel("Cube");
		}
	}

	Cube::~Cube()
	{
	}
}
