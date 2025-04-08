#pragma once

#include "EditorPanel.h"

namespace Flux
{
	class ContentsDrawer : public EditorPanel
	{
	public:
		ContentsDrawer();
		virtual ~ContentsDrawer() override;

		virtual int Initialise() override;
		virtual void Update(float deltaTime) override;

	private:
		// TODO: Contents Drawer Data
	};
}

