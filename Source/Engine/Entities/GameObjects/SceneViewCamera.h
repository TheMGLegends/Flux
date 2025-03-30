#pragma once

#include "GameObject.h"

namespace Flux
{
	class Camera;

	class SceneViewCamera : public GameObject
	{
	public:
		SceneViewCamera();
		virtual ~SceneViewCamera() override;

		virtual void LateUpdate(float deltaTime) override;

		inline std::weak_ptr<Camera> GetCamera() const { return camera; }

	private:
		std::weak_ptr<Camera> camera;

		float rotationSpeed;
		float movementSpeed;
		float minMovementSpeed;
		float maxMovementSpeed;
		DirectX::SimpleMath::Vector2 pitchConstraints;
	};
}

