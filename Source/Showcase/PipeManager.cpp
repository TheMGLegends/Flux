#include "PipeManager.h"

#include "Pipe.h"

#include "Engine/Scene/SceneContext.h"

using namespace DirectX::SimpleMath;

namespace Flux
{
	PipeManager::PipeManager() : topPipe(nullptr), bottomPipe(nullptr), pipeSpeed(5.0f)
	{
	}

	PipeManager::~PipeManager()
	{
	}

	void PipeManager::Start()
	{
		if (auto t = transform.lock())
		{
			Vector3 position(25.0f, 0.0f, 10.0f);
			t->SetPosition(position);

			// INFO: Top Pipe Creation
			topPipe = SceneContext::SpawnGameObject<Pipe>(position, Quaternion::CreateFromYawPitchRoll(0.0f, 0.0f, DirectX::XM_PI));
			topPipe->SetName("TopPipe");

			// INFO: Bottom Pipe Creation
			bottomPipe = SceneContext::SpawnGameObject<Pipe>(position, Quaternion::Identity);
			bottomPipe->SetName("BottomPipe");

		}
	}

	void PipeManager::Update(float deltaTime)
	{
		if (topPipe && bottomPipe)
		{
			if (auto t = topPipe->transform.lock())
			{
				Vector3 position = t->GetPosition();
				position.x -= pipeSpeed * deltaTime;
				t->SetPosition(position);
			}
			if (auto t = bottomPipe->transform.lock())
			{
				Vector3 position = t->GetPosition();
				position.x -= pipeSpeed * deltaTime;
				t->SetPosition(position);
			}
		}
	}
}
