#include "DetailsPanel.h"

#include <imgui.h>

#include "SceneHierarchy.h"
#include "Core/GlobalDefines.h"

using namespace Flux;
using namespace Flux::GlobalDefines;

DetailsPanel::DetailsPanel(SceneHierarchy* _sceneHierarchy) : sceneHierarchy(_sceneHierarchy)
{
}

DetailsPanel::~DetailsPanel()
{
}

int DetailsPanel::Initialise()
{
	return FLUX_SUCCESS;
}

void DetailsPanel::Update(float deltaTime)
{
	ImGui::Begin("Details Panel");
	ImGui::Text("Hello, World!");
	ImGui::End();
}
