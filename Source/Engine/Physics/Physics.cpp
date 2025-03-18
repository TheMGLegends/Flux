#include "Physics.h"

#include "Core/Debug/Debug.h"

using namespace Flux;

physx::PxFoundation* Physics::foundation = nullptr;
physx::PxDefaultAllocator Physics::defaultAllocatorCallback;
physx::PxDefaultErrorCallback Physics::defaultErrorCallback;

physx::PxPhysics* Physics::physics = nullptr;
physx::PxMaterial* Physics::defaultPhysicsMaterial = nullptr;

bool Physics::Initialise()
{
	// INFO: Create Foundation
	foundation = PxCreateFoundation(PX_PHYSICS_VERSION, defaultAllocatorCallback, defaultErrorCallback);
	if (!foundation)
	{
		Debug::LogError("Physics::Initialise() - Failed to create PhysX Foundation");
		return false;
	}

	// INFO: Create Physics
	bool recordMemoryAllocations = true;
	physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, physx::PxTolerancesScale(), recordMemoryAllocations);
	if (!physics)
	{
		Debug::LogError("Physics::Initialise() - Failed to create PhysX Physics");
		return false;
	}

	// INFO: Create Default Physics Material
	defaultPhysicsMaterial = physics->createMaterial(0.5f, 0.5f, 0.1f);

	return true;
}

void Physics::Release()
{
	if (physics)
	{
		physics->release();
		physics = nullptr;
	}

	if (foundation)
	{
		foundation->release();
		foundation = nullptr;
	}
}
