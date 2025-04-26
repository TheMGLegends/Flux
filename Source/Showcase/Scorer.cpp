#include "Scorer.h"

#include "Player.h"

#include "Engine/Audio/Audio.h"
#include "Engine/Entities/Components/Colliders/BoxCollider.h"

namespace Flux
{
	Scorer::Scorer() : score(0)
	{
		if (auto boxCollider = AddComponent<BoxCollider>(this).lock())
		{
			boxCollider->SetIsTrigger(true);
		}
	}

	Scorer::~Scorer()
	{
	}

	void Scorer::OnTriggerEnter(std::shared_ptr<Collider> other)
	{
		if (auto player = dynamic_cast<Player*>(other->GetGameObject()))
		{
			if (player->IsDead()) { return; }

			score++;
			Debug::Log(std::format("Score: {}", score));
			Audio::PlaySound2D("FlappyBirdPoint", 0.75f);
		}
	}
}
