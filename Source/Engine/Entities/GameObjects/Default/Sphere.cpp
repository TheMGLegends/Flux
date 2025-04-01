#include "Sphere.h"

#include "Engine/Entities/Components/Visualizer.h"

using namespace Flux;

Sphere::Sphere()
{
	auto visualizer = AddComponent<Visualizer>(this).lock();
	visualizer->SetModel("Sphere");
}

Sphere::~Sphere()
{
}
