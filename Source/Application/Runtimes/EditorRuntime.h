#pragma once

#include "Runtime.h"

struct ID3D11Device;
struct ID3D11DeviceContext;
struct SDL_Window;

namespace Flux
{
	class EditorRuntime : public Runtime
	{
	public:
		EditorRuntime();
		virtual ~EditorRuntime() override;

		virtual void Release() override;

		/// @brief Used for initializing systems before the main initialisation
		bool PreInitialise(SDL_Window* window, ID3D11Device& device, ID3D11DeviceContext& deviceContext);

		/// @brief Used for initializing data that uses the systems
		bool Initialise();

		void Update(float deltaTime);

	private:
		// TODO: Editor Specific Members
	};
}

