#include "Player.h"

#include "Barrier.h"
#include "Pipe.h"
#include "Scorer.h"

#include "Core/Input/Input.h"

#include "Engine/Audio/Audio.h"
#include "Engine/Entities/Components/PhysicsBody.h"
#include "Engine/Entities/Components/Colliders/SphereCollider.h"
#include "Engine/Entities/Components/Visualizer.h"
#include "Engine/Scene/SceneContext.h"

using namespace DirectX::SimpleMath;

namespace Flux
{
	Player::Player() : jumpForce(5.0f), deathDelay(2.0f), deathTimer(0.0f), isJumping(false), isDead(false)
	{
		sphereCollider = AddComponent<SphereCollider>(this);

		physicsBody = AddComponent<PhysicsBody>(this);
		visualizer = AddComponent<Visualizer>(this);
	}

	Player::~Player()
	{
	}

	void Player::Update(float deltaTime)
	{
		if (isDead)
		{
			deathTimer += deltaTime;

			if (deathTimer >= deathDelay)
			{
				deathTimer = 0.0f;
				SceneContext::LoadScene("ShowcaseLevel");
			}

			return;
		}

		if (Input::GetKeyDown(SDL_SCANCODE_SPACE))
		{
			isJumping = true;

			Audio::PlaySound2D("FlappyBirdSwing", 0.75f);
		}
	}

	void Player::FixedUpdate(float fixedDeltaTime)
	{
		if (isDead) { return; }

		if (isJumping)
		{
			isJumping = false;

			if (auto pb = physicsBody.lock())
			{
				pb->AddForce({ 0.0f, jumpForce, 0.0f }, physx::PxForceMode::eIMPULSE);
			}
		}

	}

	void Player::OnCollisionEnter(std::shared_ptr<Collider> other)
	{
		if (isDead) { return; }

		if (dynamic_cast<Pipe*>(other->GetGameObject()))
		{
			isDead = true;
			Audio::PlaySound2D("FlappyBirdHit", 0.75f);
		}
	}

	void Player::OnTriggerExit(std::shared_ptr<Collider> other)
	{
		if (isDead) { return; }

		if (dynamic_cast<Barrier*>(other->GetGameObject()))
		{
			isDead = true;
			Audio::PlaySound2D("FlappyBirdDie", 0.75f);
		}
	}
}
