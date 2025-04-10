#pragma once

#include "Runtime.h"

#include <memory>
#include <vector>

struct ID3D11Device;
struct ID3D11DeviceContext;
struct SDL_Window;

namespace Flux
{
	class ContentsDrawer;
	class DetailsPanel;
	class EditorPanel;
	class Renderer;
	class SceneHierarchy;
	class SceneView;

	class EditorRuntime : public Runtime
	{
	public:
		EditorRuntime();
		virtual ~EditorRuntime() override;

		virtual void Release() override;

		/// @brief Used for initializing systems before the main initialisation
		/// @return 0 if successful
		int PreInitialise(SDL_Window* window, ID3D11Device& device, ID3D11DeviceContext& deviceContext);

		/// @brief Used for initializing data that uses the systems
		/// @return 0 if successful
		int Initialise(Renderer& renderer);

		void Update(float deltaTime);

	private:
		/// @brief Set the custom style for ImGui
		void SetCustomStyle();

	private:
		std::vector<std::unique_ptr<EditorPanel>> editorPanels;

		SceneHierarchy* sceneHierarchy;
		SceneView* sceneView;
		DetailsPanel* detailsPanel;
		ContentsDrawer* contentsDrawer;
	};
}

