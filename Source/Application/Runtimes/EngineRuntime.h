#pragma once

#include <memory>

namespace Flux
{
	class Scene;

	class EngineRuntime
	{
	public:
		EngineRuntime();
		~EngineRuntime();

		int PreInitialise() const;
		int Initialise();

		void Start() const;
		void Update(float deltaTime);
		void FixedUpate(float fixedDeltaTime) const;
		void LateUpdate(float deltaTime);

		void Release();

		Scene& GetScene();

	private:
		void UpdateAudioSystem();

	private:
		std::unique_ptr<Scene> scene;
	};
}

