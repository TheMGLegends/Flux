#include "ContentsDrawer.h"

#include <imgui.h>

#include "Core/GlobalDefines.h"
#include "Core/Configs/RuntimeConfig.h"

using namespace Flux;
using namespace Flux::GlobalDefines;

ContentsDrawer::ContentsDrawer()
{
}

ContentsDrawer::~ContentsDrawer()
{
}

int ContentsDrawer::Initialise()
{
	return FLUX_SUCCESS;
}

void ContentsDrawer::Update(float deltaTime)
{
	if (ImGui::Begin("Contents Drawer"))
	{
		ImVec2 windowPos = ImGui::GetWindowPos();
		ImVec2 windowSize = ImGui::GetWindowSize();

		// INFO: Play Mode Overlay
		if (RuntimeConfig::IsInPlayMode() || RuntimeConfig::IsPaused())
		{
			ImGui::GetForegroundDrawList()->AddRectFilled(windowPos, ImVec2(windowPos.x + windowSize.x, windowPos.y + windowSize.y), IM_COL32(0, 116, 188, 50));
		}


		ImGui::End();
	}
}
