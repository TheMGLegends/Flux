#pragma once

#include <memory>
#include <vector>

struct ID3D11Device;
struct ID3D11DeviceContext;
struct SDL_Window;

namespace Flux
{
	class EditorPanel;
	class Renderer;

	class EditorRuntime
	{
	public:
		EditorRuntime();
		~EditorRuntime();

		int PreInitialise(SDL_Window* window, ID3D11Device& device, ID3D11DeviceContext& deviceContext) const;
		int Initialise(Renderer& renderer);
		void Update(float deltaTime) const;

		void Release() const;

	private:
		/// @brief Set the custom style for ImGui
		void SetCustomStyle() const;

	private:
		std::vector<std::unique_ptr<EditorPanel>> editorPanels;
	};
}

