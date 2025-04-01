#pragma once

#include "EditorPanel.h"
#include "Core/EventSystem/IEventListener.h"

namespace Flux
{
	class SceneHierarchy : public EditorPanel, public IEventListener
	{
	public:
		SceneHierarchy();
		virtual ~SceneHierarchy() override;

		virtual int Initialise() override;
		virtual void Update(float deltaTime) override;

		virtual void OnNotify(EventType eventType, std::shared_ptr<Event> event) override;

	private:
		// TODO: Scene Hierarchy Data
	};
}

