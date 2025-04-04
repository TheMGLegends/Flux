#pragma once

#pragma warning (push)
#pragma warning (disable : 26495) // INFO: Disable warning for uninitialised variables
#include <PxPhysicsAPI.h>
#pragma warning (pop)

namespace Flux
{
	class Physics
	{
	public:
		Physics() = delete;
		~Physics() = delete;
		Physics(const Physics&) = delete;
		Physics& operator=(const Physics&) = delete;

		/// return 0 if successful 
		static int Initialise();

		static void Release();

		static physx::PxFilterFlags CustomFilterShader(physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0,
													   physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1,
													   physx::PxPairFlags& pairFlags, const void* constantBlock, physx::PxU32 constantBlockSize);

		static inline physx::PxPhysics& GetPhysics() { return *physics; }
		static inline const physx::PxMaterial& GetDefaultPhysicsMaterial() { return *defaultPhysicsMaterial; }

	private:
		static physx::PxFoundation* foundation;
		static physx::PxDefaultAllocator defaultAllocatorCallback;
		static physx::PxDefaultErrorCallback defaultErrorCallback;

		static physx::PxPhysics* physics;
		static physx::PxMaterial* defaultPhysicsMaterial;
	};
}

