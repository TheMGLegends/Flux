#include "SceneView.h"

#include <imgui.h>

#include "Core/Configs/EditorConfig.h"
#include "Core/Configs/EngineConfig.h"
#include "Core/EventSystem/EventDispatcher.h"
#include "Core/Renderer/Renderer.h"

using namespace Flux;

SceneView::SceneView(Renderer& _renderer) : renderer(_renderer)
{
}

SceneView::~SceneView()
{
}

void SceneView::Initialise()
{
}

void SceneView::Update(float deltaTime)
{
	if (ImGui::Begin("Scene View"))
	{
		ImVec2 sceneViewSize = ImGui::GetWindowSize();
		MaintainAspectRatio(sceneViewSize);

		// INFO: Centre Scene View
		ImGui::SetCursorPos({ (ImGui::GetWindowSize().x - sceneViewSize.x) * 0.5f, (ImGui::GetWindowSize().y - sceneViewSize.y) * 0.5f });

		ImGui::Image((ImTextureID)renderer.GetRenderTextureShaderResourceView(), sceneViewSize);

		// INFO: Check and Update Scene View Size if Necessary
		if (EditorConfig::sceneViewWidth != sceneViewSize.x || EditorConfig::sceneViewHeight != sceneViewSize.y)
		{
			EditorConfig::sceneViewWidth = sceneViewSize.x;
			EditorConfig::sceneViewHeight = sceneViewSize.y;
			EventDispatcher::QueueEvent(EventType::SceneViewResized, nullptr);
		}
	}

	ImGui::End();
}

void SceneView::OnNotify(EventType eventType, std::shared_ptr<Event> event)
{
}

void SceneView::MaintainAspectRatio(ImVec2& sceneViewSize)
{
	// INFO: Maintain Aspect Ratio
	float adheringWidth = sceneViewSize.y * EngineConfig::ASPECT_RATIO;
	float adheringHeight = sceneViewSize.x * (1.0f / EngineConfig::ASPECT_RATIO);

	if (sceneViewSize.x >= adheringWidth)
		sceneViewSize.x = adheringWidth;
	else
		sceneViewSize.y = adheringHeight;

	// INFO: Adjust for Top Bar and Scroll Bar
	sceneViewSize.x -= 15.0f;
	sceneViewSize.y -= 40.0f;
}
