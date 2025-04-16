#pragma once

#include "EditorPanel.h"
#include "Core/EventSystem/IEventListener.h"

#include <string>

struct ImFont;
struct ImVec2;

namespace Flux
{
	class GameObject;
	class Scene;

	class SceneHierarchy : public EditorPanel, public IEventListener
	{
	public:
		SceneHierarchy();
		~SceneHierarchy() override;

		int Initialise() override;
		void Update(float deltaTime) override;

		void OnNotify(EventType eventType, std::shared_ptr<Event> event) override;

		GameObject* GetSelectedGameObject() const;

	private:
		void DisplaySceneObjects(const ImVec2& windowSize);
		void GameSettingsPopup() const;
		void AddGameObjectButton(float windowWidth);

	private:
		Scene& scene;
		GameObject* selectedGameObject;

		bool isRenaming;
		std::string searchString;

		ImFont* bigFont;
	};
}

