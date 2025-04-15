#pragma once

#include <memory>

namespace Flux
{
	class Scene;

	class EngineRuntime
	{
	public:
		EngineRuntime() = default;
		~EngineRuntime() = default;

		int PreInitialise();
		int Initialise();

		void Start();
		void Update(float deltaTime);
		void FixedUpate(float fixedDeltaTime);
		void LateUpdate(float deltaTime);

		void Release();

		Scene& GetScene() { return *scene.get(); }

	private:
		void UpdateAudioSystem();

	private:
		std::unique_ptr<Scene> scene;
	};
}

