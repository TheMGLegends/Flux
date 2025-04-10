#pragma once

#include <SimpleMath.h>

#include "Scene.h"

#include "Core/Debug/Debug.h"

#include "Engine/Entities/Components/Transform.h"

namespace Flux
{
	class SceneContext
	{
	public:
		SceneContext() = delete;
		~SceneContext() = delete;
		SceneContext(const SceneContext&) = delete;
		SceneContext& operator=(const SceneContext&) = delete;

		static void SetScene(Scene* _scene) { scene = _scene; }
		static Scene& GetScene() { return *scene; }

		template<class T, typename... Args>
		static T* SpawnGameObject(const DirectX::SimpleMath::Vector3& spawnLocation, const DirectX::SimpleMath::Quaternion& spawnRotation, Args&&... args);

	private:
		static Scene* scene;
	};

	template<class T, typename ...Args>
	inline T* SceneContext::SpawnGameObject(const DirectX::SimpleMath::Vector3& spawnLocation, const DirectX::SimpleMath::Quaternion& spawnRotation, Args && ...args)
	{
		// INFO: Ensure T is a derived class of GameObject
		static_assert(std::is_base_of<GameObject, T>::value, "T must derive from GameObject");

		if (!scene)
		{
			Debug::LogError("SceneContext::SpawnGameObject - Scene is not set!");
			return nullptr;
		}

		// INFO: Add GameObject to Scene
		scene->gameObjects.emplace_back(std::make_unique<T>(std::forward<Args>(args)...));

		// INFO: Get a reference to the newly created GameObject
		T* newGameObject = dynamic_cast<T*>(scene->gameObjects.back().get());

		// INFO: Set the GameObject's transform properties
		if (!newGameObject->transform.expired(); std::shared_ptr<Transform> newGameObjectTransform = newGameObject->transform.lock())
		{
			newGameObjectTransform->SetPosition(spawnLocation);
			newGameObjectTransform->SetRotation(spawnRotation);
		}

		return newGameObject;
	}
}

