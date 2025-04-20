#include "EngineRuntime.h"

#include "Core/GlobalDefines.h"

#include "Core/Configs/GameConfig.h"
#include "Core/Configs/RuntimeConfig.h"

#include "Core/Debug/Debug.h"
#include "Core/Time/Time.h"

#include "Engine/Audio/Audio.h"
#include "Engine/Physics/Physics.h"
#include "Engine/Scene/Scene.h"

#include "Engine/Entities/Components/Camera.h"
#include "Engine/Entities/Components/Transform.h"
#include "Engine/Entities/GameObjects/GameObject.h"

using namespace DirectX::SimpleMath;

namespace Flux
{
	using namespace GlobalDefines;

	EngineRuntime::EngineRuntime() : scene(nullptr)
	{
	}

	EngineRuntime::~EngineRuntime() = default;

	int EngineRuntime::PreInitialise() const
	{
		if (FLUX_FAIL(Physics::Initialise()))
		{
			Debug::LogError("EngineRuntime::PreInitialise() - Failed to initialise Physics System");
			return FLUX_FAILURE;
		}

		if (FLUX_FAIL(Audio::Initialise()))
		{
			Debug::LogError("EngineRuntime::PreInitialise() - Failed to initialise Audio System");
			return FLUX_FAILURE;
		}

		return FLUX_SUCCESS;
	}

	int EngineRuntime::Initialise()
	{
		// INFO: Deserialize Game Settings
		GameConfig::DeserializeGameConfig();

		scene = std::make_unique<Scene>();

		if (!scene)
		{
			Debug::LogError("EngineRuntime::Initialise() - Failed to create Scene");
			return FLUX_FAILURE;
		}

		return FLUX_SUCCESS;
	}

	void EngineRuntime::Start() const
	{
		scene->Start();
	}

	void EngineRuntime::Update(float deltaTime)
	{
		scene->Update(deltaTime);
	}

	void EngineRuntime::FixedUpate(float fixedDeltaTime) const
	{
		while (Time::PerformPhysicsUpdate())
		{
			scene->GetPhysicsScene().simulate(fixedDeltaTime);
			scene->FixedUpdate(fixedDeltaTime);
			scene->GetPhysicsScene().fetchResults(true);
			Time::DecrementAccumulator();
		}
	}

	void EngineRuntime::LateUpdate(float deltaTime)
	{
		scene->LateUpdate(deltaTime);
		UpdateAudioSystem();
	}

	void EngineRuntime::Release()
	{
		scene.reset();

		Audio::Release();
		Physics::Release();
	}

	Scene& EngineRuntime::GetScene()
	{
		return *scene.get();
	}

	void EngineRuntime::UpdateAudioSystem()
	{
		// INFO: Update Audio System and Current Listener if in Play Mode
		if (RuntimeConfig::IsInPlayMode() && !RuntimeConfig::IsPaused())
		{
			if (std::shared_ptr<Camera> camera = scene->GetCamera().lock())
			{
				std::shared_ptr<Transform> cameraTransform = camera->GetGameObject()->transform.lock();

				Vector3 position = cameraTransform->GetPosition();
				Vector3 forward = cameraTransform->Forward();
				Vector3 up = cameraTransform->Up();
				Audio::SetListenerAttributes({ position.x, position.y, position.z }, { 0.0f, 0.0f, 0.0f },
					{ forward.x, forward.y, forward.z }, { up.x, up.y, up.z });
			}

			Audio::Update();
		}
	}
}
