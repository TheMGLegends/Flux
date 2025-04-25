#include "Pipe.h"

#include "Engine/Entities/Components/Visualizer.h"
#include "Engine/Entities/Components/Colliders/BoxCollider.h"

using namespace DirectX::SimpleMath;

namespace Flux
{
	Pipe::Pipe()
	{
		if (auto visualizer = AddComponent<Visualizer>(this).lock())
		{
			visualizer->SetMaterialTexture("Green");
			visualizer->SetModel("Pipe");
		}

		if (auto boxCollider = AddComponent<BoxCollider>(this).lock())
		{
			boxCollider->SetSize(Vector3(0.9f, 3.5f, 1.0f));
		}
	}

	Pipe::~Pipe()
	{
	}
}
