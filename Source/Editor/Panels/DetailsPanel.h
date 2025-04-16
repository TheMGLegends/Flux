#pragma once

#include "EditorPanel.h"

#include <memory>

struct ImFont;

namespace Flux
{
	class Component;
	class GameObject;
	class SceneHierarchy;

	class DetailsPanel : public EditorPanel
	{
	public:
		explicit DetailsPanel(SceneHierarchy* _sceneHierarchy);
		~DetailsPanel() override;

		int Initialise() override;
		void Update(float deltaTime) override;

	private:
		void AddComponentPopup(GameObject* selectedGameObject, int componentCount, float windowWidth) const;
		void ComponentAlreadyExists(int oldComponentCount, int newComponentCount, std::weak_ptr<Component> component) const;

	private:
		SceneHierarchy* sceneHierarchy;
		ImFont* bigFont;
	};
}

