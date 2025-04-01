#include "Cube.h"

#include "Engine/Entities/Components/Visualizer.h"

using namespace Flux;

Cube::Cube()
{
	auto visualizer = AddComponent<Visualizer>(this).lock();
	visualizer->SetModel("Cube");
}

Cube::~Cube()
{
}
