#pragma once

#include "Engine/Interfaces/ISerializable.h"

#include <memory>
#include <vector>

namespace Flux
{
	class GameObject;
	class Camera;

	class Scene : public ISerializable
	{
		friend class SceneContext;

	public:
		Scene();
		~Scene();

		virtual void Serialize(nlohmann::ordered_json& json) const override;
		virtual void Deserialize(const nlohmann::ordered_json& json) override;

		void Start();
		void Update(float deltaTime);
		void LateUpdate(float deltaTime);
		void FixedUpdate(float fixedDeltaTime);

		inline std::shared_ptr<Camera> GetCamera() const { return camera.expired() ? nullptr : camera.lock(); }

	private:
		std::vector<std::unique_ptr<GameObject>> gameObjects;
		std::weak_ptr<Camera> camera;
	};
}

