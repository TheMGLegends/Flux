#pragma once

#include "EditorPanel.h"
#include "Core/EventSystem/IEventListener.h"

namespace Flux
{
	class SceneView : public EditorPanel, public IEventListener
	{
	public:
		SceneView();
		virtual ~SceneView() override;

		virtual void Initialise() override;
		virtual void Update(float deltaTime) override;

		virtual void OnNotify(EventType eventType, std::shared_ptr<Event> event) override;

		// TODO: Load Scene Event

	private:
		// TODO: SceneView Data
	};
}

