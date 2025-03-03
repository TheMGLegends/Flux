#pragma once

#include "Runtime.h"

namespace Flux
{
	class EditorRuntime : public Runtime
	{
	public:
		EditorRuntime();
		virtual ~EditorRuntime() override;

		/// @brief Used for initializing systems before the main initialisation
		bool PreInitialise() override;
		/// @brief Used for initializing data that uses the systems
		bool Initialise() override;
		void Update(float deltaTime) override;


		// INFO: Editor Specific Methods

		void RenderGUI();

	private:
		// TODO: Editor Specific Members
	};
}

