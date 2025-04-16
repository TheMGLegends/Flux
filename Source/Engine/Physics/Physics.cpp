#include "Physics.h"

#include "Core/GlobalDefines.h"

#include "Core/Debug/Debug.h"

namespace Flux
{
	using namespace GlobalDefines;

	physx::PxFoundation* Physics::foundation = nullptr;
	physx::PxDefaultAllocator Physics::defaultAllocatorCallback;
	physx::PxDefaultErrorCallback Physics::defaultErrorCallback;

	physx::PxPhysics* Physics::physics = nullptr;
	physx::PxMaterial* Physics::defaultPhysicsMaterial = nullptr;

	int Physics::Initialise()
	{
		// INFO: Create Foundation
		foundation = PxCreateFoundation(PX_PHYSICS_VERSION, defaultAllocatorCallback, defaultErrorCallback);

		if (!foundation)
		{
			Debug::LogError("Physics::Initialise() - Failed to create PhysX Foundation");
			return FLUX_FAILURE;
		}

		// INFO: Create Physics
		bool recordMemoryAllocations = true;
		physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, physx::PxTolerancesScale(), recordMemoryAllocations);

		if (!physics)
		{
			Debug::LogError("Physics::Initialise() - Failed to create PhysX Physics");
			return FLUX_FAILURE;
		}

		// INFO: Create Default Physics Material
		defaultPhysicsMaterial = physics->createMaterial(0.5f, 0.5f, 0.1f);

		return FLUX_SUCCESS;
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

	physx::PxFilterFlags Physics::CustomFilterShader(physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0, 
													 physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1, physx::PxPairFlags& pairFlags, 
													 const void* constantBlock, physx::PxU32 constantBlockSize)
	{
		// INFO: Let triggers through
		if (physx::PxFilterObjectIsTrigger(attributes0) || physx::PxFilterObjectIsTrigger(attributes1))
		{
			pairFlags = physx::PxPairFlag::eTRIGGER_DEFAULT;
			return physx::PxFilterFlag::eDEFAULT;
		}

		// INFO: Generate contacts for all that were not filtered above
		pairFlags = physx::PxPairFlag::eCONTACT_DEFAULT;

		// INFO: Trigger the contact callback for pairs (A,B) where
		// the filtermask of A contains the ID of B and vice versa
		if ((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
		{
			pairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_FOUND;
			pairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_LOST;
		}

		return physx::PxFilterFlag::eDEFAULT;
	}

	physx::PxPhysics& Physics::GetPhysics()
	{
		return *physics;
	}

	const physx::PxMaterial& Physics::GetDefaultPhysicsMaterial()
	{
		return *defaultPhysicsMaterial;
	}
}
