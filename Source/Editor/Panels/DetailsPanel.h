#pragma once

#include "EditorPanel.h"

#include <memory>

struct ImFont;

namespace Flux
{
	class Component;
	class SceneHierarchy;

	class DetailsPanel : public EditorPanel
	{
	public:
		DetailsPanel(SceneHierarchy* _sceneHierarchy);
		virtual ~DetailsPanel() override;

		virtual int Initialise() override;
		virtual void Update(float deltaTime) override;

	private:
		void ComponentAlreadyExists(int oldComponentCount, int newComponentCount, std::weak_ptr<Component> component);

	private:
		SceneHierarchy* sceneHierarchy;
		ImFont* bigFont;
	};
}

