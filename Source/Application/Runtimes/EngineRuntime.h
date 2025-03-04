#pragma once

#include "Runtime.h"

#include "Engine/Scene/Scene.h"

namespace Flux
{
	class Scene;

	class EngineRuntime : public Runtime
	{
	public:
		EngineRuntime();
		virtual ~EngineRuntime() override;

		/// @brief Used for initializing systems before the main initialisation
		bool PreInitialise() override;
		/// @brief Used for initializing data that uses the systems
		bool Initialise() override;
		void Update(float deltaTime) override;


		// INFO: Engine Specific Methods

		void Start();
		void FixedUpate(float fixedDeltaTime);

		inline Scene& GetScene() { return scene; }

	private:
		Scene scene;
		// TODO: Engine Specific Members
	};
}

