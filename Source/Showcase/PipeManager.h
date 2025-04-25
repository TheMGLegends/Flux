#pragma once

#include "Engine/Entities/GameObjects/GameObject.h"

namespace Flux
{
	class Pipe;

	class PipeManager : public GameObject
	{
		REFLECT(PipeManager);

	public:
		PipeManager();
		~PipeManager() override;

		void Start() override;
		void Update(float deltaTime) override;

	private:
		Pipe* topPipe;
		Pipe* bottomPipe;

		float pipeSpeed;
	};
}

