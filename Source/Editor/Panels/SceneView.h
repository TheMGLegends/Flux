#pragma once

#include "EditorPanel.h"

#include <imgui.h>

namespace Flux
{
	class Renderer;
	class SceneHierarchy;

	class SceneView : public EditorPanel
	{
	public:
		SceneView(Renderer& _renderer, SceneHierarchy* _sceneHierarchy);
		~SceneView() override;

		int Initialise() override;
		void Update(float deltaTime) override;

	private:
		void MaintainAspectRatio(ImVec2& sceneViewSize) const;
		void DrawGizmos(const ImVec2& windowSize) const;
		void DrawGizmoButtons(const ImVec2& windowSize, const ImVec2& sceneViewSize, ImGuiWindowFlags windowFlags) const;
		void DrawSceneViewButtons(const ImVec2& windowSize, const ImVec2& sceneViewSize, ImGuiWindowFlags windowFlags) const;

	private:
		Renderer& renderer;
		SceneHierarchy* sceneHierarchy;

		ImTextureID panTexture;
		ImTextureID panTextureSelected;

		ImTextureID translationTexture;
		ImTextureID translationTextureSelected;

		ImTextureID rotationTexture;
		ImTextureID rotationTextureSelected;

		ImTextureID scaleTexture;
		ImTextureID scaleTextureSelected;

		ImTextureID playButtonEditorMode;
		ImTextureID playButtonPlayMode;

		ImTextureID pauseButtonUnavailable;
		ImTextureID pauseButtonSelected;
		ImTextureID pauseButtonAvailable;
	};
}

