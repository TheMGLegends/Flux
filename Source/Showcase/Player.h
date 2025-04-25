#pragma once

#include "Engine/Entities/GameObjects/GameObject.h"

namespace Flux
{
	class PhysicsBody;
	class SphereCollider;
	class Visualizer;

	class Player : public GameObject
	{
		REFLECT(Player);

	public:
		Player();
		~Player() override;

		void Update(float deltaTime) override;
		void FixedUpdate(float fixedDeltaTime) override;
		void OnTriggerExit(std::shared_ptr<Collider> other) override;

	private:
		std::weak_ptr<PhysicsBody> physicsBody;
		std::weak_ptr<SphereCollider> sphereCollider;
		std::weak_ptr<Visualizer> visualizer;

		float jumpForce;
		float torqueScale;
		bool isJumping;
		bool isDead;
	};
}

