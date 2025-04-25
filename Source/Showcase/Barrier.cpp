#include "Barrier.h"

#include "Engine/Entities/Components/Colliders/BoxCollider.h"

namespace Flux
{
	Barrier::Barrier()
	{
		if (auto boxCollider = AddComponent<BoxCollider>(this).lock())
		{
			boxCollider->SetIsTrigger(true);
		}
	}

	Barrier::~Barrier()
	{
	}
}
