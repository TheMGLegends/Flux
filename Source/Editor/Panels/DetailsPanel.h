#pragma once

#include "EditorPanel.h"

namespace Flux
{
	class SceneHierarchy;

	class DetailsPanel : public EditorPanel
	{
	public:
		DetailsPanel(SceneHierarchy* _sceneHierarchy);
		virtual ~DetailsPanel() override;

		virtual int Initialise() override;
		virtual void Update(float deltaTime) override;

	private:
		SceneHierarchy* sceneHierarchy;
	};
}

