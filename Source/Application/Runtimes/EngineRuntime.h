#pragma once

#include "Runtime.h"

#include <memory>

namespace Flux
{
	class Scene;

	class EngineRuntime : public Runtime
	{
	public:
		EngineRuntime();
		virtual ~EngineRuntime() override;

		virtual void Release() override;

		int PreInitialise();
		int Initialise();
		void Start();
		void Update(float deltaTime);
		void FixedUpate(float fixedDeltaTime);
		void LateUpdate(float deltaTime);

		Scene& GetScene() { return *scene.get(); }

	private:
		std::unique_ptr<Scene> scene;
	};
}

