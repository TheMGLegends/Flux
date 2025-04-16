#pragma once

#include "GameObject.h"

namespace Flux
{
	class Camera;

	class SceneViewCamera : public GameObject
	{
	public:
		SceneViewCamera();
		~SceneViewCamera() override;

		void LateUpdate(float deltaTime) override;

		std::weak_ptr<Camera> GetCamera() const;

	private:
		std::weak_ptr<Camera> camera;

		float rotationSpeed;
		float movementSpeed;
		float minMovementSpeed;
		float maxMovementSpeed;
		DirectX::SimpleMath::Vector2 pitchConstraints;
	};
}

