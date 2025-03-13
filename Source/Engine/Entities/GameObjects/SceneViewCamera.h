#pragma once

#include "GameObject.h"

namespace Flux
{
	class Camera;

	class SceneViewCamera : public GameObject
	{
		// TODO: TEST
		REFLECT(SceneViewCamera)

	public:
		SceneViewCamera();
		virtual ~SceneViewCamera() override;

		virtual void Update(float deltaTime) override;

		inline std::weak_ptr<Camera> GetCamera() const { return camera; }

	private:
		std::weak_ptr<Camera> camera;

		float rotationSpeed;
		float movementSpeed;
		DirectX::SimpleMath::Vector2 pitchConstraints;
	};
}

