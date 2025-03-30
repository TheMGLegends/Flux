#pragma once

#include "EditorPanel.h"
#include "Core/EventSystem/IEventListener.h"

#include <imgui.h>

namespace Flux
{
	class Renderer;

	class SceneView : public EditorPanel, public IEventListener
	{
	public:
		SceneView(Renderer& _renderer);
		virtual ~SceneView() override;

		virtual int Initialise() override;
		virtual void Update(float deltaTime) override;

		virtual void OnNotify(EventType eventType, std::shared_ptr<Event> event) override;

		// TODO: Load Scene Event

	private:
		void MaintainAspectRatio(ImVec2& sceneViewSize);

	private:
		Renderer& renderer;

		// TODO: SceneView Data
	};
}

