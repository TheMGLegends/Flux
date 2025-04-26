#pragma once

#include "Engine/Entities/GameObjects/GameObject.h"

namespace Flux
{
	class Scorer : public GameObject
	{
	public:
		Scorer();
		~Scorer() override;

		void OnTriggerEnter(std::shared_ptr<Collider> other) override;

	private:
		int score;
	};
}

