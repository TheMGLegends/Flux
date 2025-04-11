#include "Cube.h"

#include "Engine/Entities/Components/Visualizer.h"

#include "Core/Input/Input.h"

#include "Engine/Scene/SceneContext.h"

namespace Flux
{
	Cube::Cube()
	{
		if (std::shared_ptr<Visualizer> visualizer = AddComponent<Visualizer>(this).lock())
		{
			visualizer->SetModel("Cube");
		}
	}

	Cube::~Cube()
	{
	}

	void Cube::Update(float deltaTime)
	{
		if (Input::GetKeyDown(SDL_SCANCODE_L))
		{
			SceneContext::LoadScene("TestScene");
		}
	}
}
