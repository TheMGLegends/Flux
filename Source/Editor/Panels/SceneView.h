#pragma once

#include "EditorPanel.h"
#include "Core/EventSystem/IEventListener.h"

struct ImVec2;

namespace Flux
{
	class Renderer;

	class SceneView : public EditorPanel, public IEventListener
	{
	public:
		SceneView(Renderer& _renderer);
		virtual ~SceneView() override;

		virtual void Initialise() override;
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

