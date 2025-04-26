#pragma once

#include "Engine/Entities/GameObjects/GameObject.h"

namespace Flux
{
	class Pipe;
	class Player;
	class Scorer;

	class PipeManager : public GameObject
	{
		REFLECT(PipeManager);

	public:
		PipeManager();
		~PipeManager() override;

		void Start() override;
		void Update(float deltaTime) override;

		void RandomizePipes();
		void ResetPipes(float xPosition);

	private:
		Pipe* topPipe;
		Scorer* scorer;
		Pipe* bottomPipe;
		Player* player;

		float pipeSpeed;
	};
}

