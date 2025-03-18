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

		static bool Initialise();
		static void Release();

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

