#include "PipeManager.h"

#include "Pipe.h"
#include "Player.h"
#include "Scorer.h"

#include "Core/Helpers/MathHelpers.h"

#include "Engine/Scene/SceneContext.h"

using namespace DirectX::SimpleMath;

constexpr Vector2 TOP_PIPE_POSITION_RANGE(2.0f, 15.0f);
constexpr Vector2 PIPE_GAP_RANGE(1.5f, 3.0f);
constexpr float HALF_PIPE_HEIGHT = 3.375f;

namespace Flux
{
	PipeManager::PipeManager() : topPipe(nullptr), scorer(nullptr), bottomPipe(nullptr), player(nullptr), pipeSpeed(5.0f)
	{
	}

	PipeManager::~PipeManager()
	{
	}

	void PipeManager::Start()
	{
		// INFO: Get Player Reference
		player = SceneContext::FindFirstGameObjectByName<Player>("Player");

		if (auto t = transform.lock())
		{
			Vector3 position(21.0f, 0.0f, 10.0f);
			t->SetPosition(position);

			Vector3 pipeScale(1.0f, 2.0f, 1.0f);

			// INFO: Top Pipe Creation
			topPipe = SceneContext::SpawnGameObject<Pipe>(position, Quaternion::CreateFromYawPitchRoll(0.0f, 0.0f, DirectX::XM_PI));
			topPipe->SetName("TopPipe");
			topPipe->transform.lock()->SetScale(pipeScale);

			// INFO: Scorer Creation
			scorer = SceneContext::SpawnGameObject<Scorer>(position, Quaternion::Identity);
			scorer->SetName("Scorer");
			scorer->GetComponent<BoxCollider>().lock()->SetSize(Vector3(0.75f, 1.0f, 1.0f));

			// INFO: Bottom Pipe Creation
			bottomPipe = SceneContext::SpawnGameObject<Pipe>(position, Quaternion::Identity);
			bottomPipe->SetName("BottomPipe");
			bottomPipe->transform.lock()->SetScale(pipeScale);

			// INFO: Initial Pipe Randomization
			RandomizePipes();
		}
	}

	void PipeManager::Update(float deltaTime)
	{
		// INFO: Stop Pipe Movement if Player is Dead
		if (player && player->IsDead()) { return; }

		if (topPipe && bottomPipe)
		{
			if (auto t = topPipe->transform.lock())
			{
				Vector3 position = t->GetPosition();
				position.x -= pipeSpeed * deltaTime;
				t->SetPosition(position);
			}

			if (auto t = scorer->transform.lock())
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

				// INFO: Can check any element since they are all in the same position (X-Axis)
				if (t->GetPosition().x <= -21.0f)
				{
					// INFO: Pipe Manager remains at starting position
					ResetPipes(transform.lock()->GetPosition().x);
				}
			}


		}
	}

	void PipeManager::RandomizePipes()
	{
		// INFO: Randomize Position of Top Pipe
		if (auto t = topPipe->transform.lock())
		{
			Vector3 position = t->GetPosition();
			position.y = MathHelpers::RandomRange(TOP_PIPE_POSITION_RANGE.x, TOP_PIPE_POSITION_RANGE.y);
			t->SetPosition(position);
		}

		// INFO: Randomize Position of Scorer based on Top Pipe Position
		if (auto t = scorer->transform.lock())
		{
			float scorerYScale = MathHelpers::RandomRange(PIPE_GAP_RANGE.x, PIPE_GAP_RANGE.y);
			Vector3 scale = t->GetScale();
			t->SetScale(Vector3(scale.x, scorerYScale, scale.z));

			Vector3 position = t->GetPosition();
			float topPipeY = topPipe->transform.lock()->GetPosition().y;
			float pipeScaleY = topPipe->transform.lock()->GetScale().y;

			position.y = topPipeY - (HALF_PIPE_HEIGHT * pipeScaleY) - t->GetScale().y;
			t->SetPosition(position);
		}

		// INFO: Randomize Position of Bottom Pipe
		if (auto t = bottomPipe->transform.lock())
		{
			float scorerYPosition = scorer->transform.lock()->GetPosition().y;
			float scorerYScale = scorer->transform.lock()->GetScale().y;

			Vector3 position = t->GetPosition();
			position.y = scorerYPosition - scorerYScale - (HALF_PIPE_HEIGHT * t->GetScale().y);
			t->SetPosition(position);
		}
	}

	void PipeManager::ResetPipes(float xPosition)
	{
		if (auto t = topPipe->transform.lock())
		{
			Vector3 position = t->GetPosition();
			position.x = xPosition;
			t->SetPosition(position);
		}

		if (auto t = bottomPipe->transform.lock())
		{
			Vector3 position = t->GetPosition();
			position.x = xPosition;
			t->SetPosition(position);
		}

		if (auto t = scorer->transform.lock())
		{
			Vector3 position = t->GetPosition();
			position.x = xPosition;
			t->SetPosition(position);
		}

		// INFO: Randomize Pipes after Reset
		RandomizePipes();
	}
}
