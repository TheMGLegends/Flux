#include "SceneHierarchy.h"

#include <imgui.h>

using namespace Flux;

SceneHierarchy::SceneHierarchy()
{
}

SceneHierarchy::~SceneHierarchy()
{
}

int SceneHierarchy::Initialise()
{
	return 0;
}

void SceneHierarchy::Update(float deltaTime)
{
	if (ImGui::Begin("Scene Hierarchy"))
	{

		ImGui::End();
	}
}

void SceneHierarchy::OnNotify(EventType eventType, std::shared_ptr<Event> event)
{
}
