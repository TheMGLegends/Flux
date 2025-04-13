#pragma once

#include "EditorPanel.h"
#include "Core/EventSystem/IEventListener.h"

#include <string>

namespace Flux
{
	class GameObject;
	class Scene;

	class SceneHierarchy : public EditorPanel, public IEventListener
	{
	public:
		SceneHierarchy();
		virtual ~SceneHierarchy() override;

		virtual int Initialise() override;
		virtual void Update(float deltaTime) override;

		virtual void OnNotify(EventType eventType, std::shared_ptr<Event> event) override;

		GameObject* GetSelectedGameObject() const;

	private:
		Scene& scene;
		GameObject* selectedGameObject;

		bool isRenaming;
		std::string searchString;
	};
}

