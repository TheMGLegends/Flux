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
		virtual ~SceneView() override;

		virtual int Initialise() override;
		virtual void Update(float deltaTime) override;

	private:
		void MaintainAspectRatio(ImVec2& sceneViewSize);

	private:
		Renderer& renderer;
		SceneHierarchy* sceneHierarchy;

		// TODO: SceneView Data
	};
}

