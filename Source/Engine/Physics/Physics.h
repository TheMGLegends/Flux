#pragma once

#include <PxPhysicsAPI.h>

namespace Flux
{
	class Physics
	{
	public:
		Physics() = delete;
		~Physics() = delete;
		Physics(const Physics&) = delete;
		Physics& operator=(const Physics&) = delete;

		static int Initialise();
		static void Release();

		static physx::PxFilterFlags CustomFilterShader(physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0,
													   physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1,
													   physx::PxPairFlags& pairFlags, const void* constantBlock, physx::PxU32 constantBlockSize);

		static physx::PxPhysics& GetPhysics();
		static const physx::PxMaterial& GetDefaultPhysicsMaterial();

	private:
		static physx::PxFoundation* foundation;
		static physx::PxDefaultAllocator defaultAllocatorCallback;
		static physx::PxDefaultErrorCallback defaultErrorCallback;

		static physx::PxPhysics* physics;
		static physx::PxMaterial* defaultPhysicsMaterial;
	};
}

