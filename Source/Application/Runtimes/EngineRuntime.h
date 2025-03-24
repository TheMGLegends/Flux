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

		/// @brief Used for initializing systems before the main initialisation
		bool PreInitialise();

		/// @brief Used for initializing data that uses the systems
		bool Initialise();

		void Start();
		void Update(float deltaTime);
		void FixedUpate(float fixedDeltaTime);
		void LateUpdate(float deltaTime);

		inline Scene& GetScene() { return *scene.get(); }

	private:
		std::unique_ptr<Scene> scene;
	};
}

