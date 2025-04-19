#include "Cube.h"

#include "Engine/Entities/Components/Visualizer.h"

#include "Engine/Entities/Components/PhysicsBody.h"

#include "Core/Input/Input.h"

using namespace DirectX::SimpleMath;
// TODO: TESTING DELETE LATER

namespace Flux
{
	Cube::Cube()
	{
		if (std::shared_ptr<Visualizer> visualizer = AddComponent<Visualizer>(this).lock())
		{
			visualizer->SetModel("Cube");
		}
	}

	Cube::~Cube() = default;

	void Cube::Update(float deltaTime)
	{
		if (Input::GetKey(SDL_SCANCODE_W))
		{
			auto physicsBody = GetComponent<PhysicsBody>().lock();

			if (physicsBody)
			{
				//physicsBody->AddForce(Vector3(10.0f * deltaTime, 0.0f, 0.0f), physx::PxForceMode::eFORCE);
				physicsBody->AddTorque(Vector3(0.0f, -10000.0f * deltaTime, 0.0f), physx::PxForceMode::eFORCE);
			}
		}
	}
}
