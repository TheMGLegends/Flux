#include "EditorRuntime.h"

#include <imgui.h>
#include <ImGuizmo.h>
#include <backends/imgui_impl_dx11.h>
#include <backends/imgui_impl_sdl3.h>

#include "Core/GlobalDefines.h"

#include "Core/Configs/EditorConfig.h"
#include "Core/Configs/RuntimeConfig.h"

#include "Core/Debug/Debug.h"
#include "Core/Debug/FrameRateMonitor.h"
#include "Core/EventSystem/EventDispatcher.h"
#include "Core/Input/Input.h"

#include "Editor/Panels/ContentsDrawer.h"
#include "Editor/Panels/DetailsPanel.h"
#include "Editor/Panels/SceneHierarchy.h"
#include "Editor/Panels/SceneView.h"

namespace Flux
{
	using namespace GlobalDefines;

	EditorRuntime::EditorRuntime()
	{
	}

	EditorRuntime::~EditorRuntime()
	{
	}

	int EditorRuntime::PreInitialise(SDL_Window* window, ID3D11Device& device, ID3D11DeviceContext& deviceContext)
	{
		// INFO: ImGui Initialisation
		IMGUI_CHECKVERSION();
		ImGuiContext* context = ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		SetCustomStyle();
		io.FontDefault = io.Fonts->AddFontFromFileTTF("Assets/Default/Fonts/OpenSans-Bold.ttf", EditorConfig::FONT_SIZE);

		if (!ImGui_ImplSDL3_InitForD3D(window))
		{
			Debug::LogError("EditorRuntime::PreInitialise() - Failed to initialise ImGui SDL3 Implementation");
			return FLUX_FAILURE;
		}

		if (!ImGui_ImplDX11_Init(&device, &deviceContext))
		{
			Debug::LogError("EditorRuntime::PreInitialise() - Failed to initialise ImGui DirectX11 Implementation");
			return FLUX_FAILURE;
		}

		// INFO: ImGuizmo Initialisation
		if (!context)
		{
			Debug::LogError("EditorRuntime::PreInitialise() - Failed to create ImGui Context");
			return FLUX_FAILURE;
		}

		ImGuizmo::SetImGuiContext(context);
		ImGuizmo::AllowAxisFlip(false); // INFO: Only draws transform gizmos in the positive direction

		return FLUX_SUCCESS;
	}

	int EditorRuntime::Initialise(Renderer& renderer)
	{
		FrameRateMonitor::Initialise();

		// INFO: Editor Panel Initialisations
		editorPanels.emplace_back(std::make_unique<SceneHierarchy>());
		SceneHierarchy* sceneHierarchy = static_cast<SceneHierarchy*>(editorPanels.back().get());

		if (!sceneHierarchy)
		{
			Debug::LogError("EditorRuntime::Initialise() - Failed to create SceneHierarchy");
			return FLUX_FAILURE;
		}

		editorPanels.emplace_back(std::make_unique<SceneView>(renderer, sceneHierarchy));

		if (!editorPanels.back())
		{
			Debug::LogError("EditorRuntime::Initialise() - Failed to create SceneView");
			return FLUX_FAILURE;
		}

		editorPanels.emplace_back(std::make_unique<DetailsPanel>(sceneHierarchy));

		if (!editorPanels.back())
		{
			Debug::LogError("EditorRuntime::Initialise() - Failed to create DetailsPanel");
			return FLUX_FAILURE;
		}

		editorPanels.emplace_back(std::make_unique<ContentsDrawer>());

		if (!editorPanels.back())
		{
			Debug::LogError("EditorRuntime::Initialise() - Failed to create ContentsDrawer");
			return FLUX_FAILURE;
		}

		for (size_t i = 0; i < editorPanels.size(); i++)
		{
			std::unique_ptr<EditorPanel>& editorPanel = editorPanels[i];

			if (FLUX_FAIL(editorPanel->Initialise()))
			{
				Debug::LogError("EditorRuntime::Initialise() - Failed to initialise EditorPanel");
				return FLUX_FAILURE;
			}
		}

		return FLUX_SUCCESS;
	}

	void EditorRuntime::Update(float deltaTime)
	{
		FrameRateMonitor::Update(deltaTime);

		// INFO: Toggle FPS Counter Visibility
		if (Input::GetKeyDown(SDL_SCANCODE_F3)) { FrameRateMonitor::Toggle(); }

		// INFO: Save Scene if CTRL+S is Pressed and in Editor Mode
		if (RuntimeConfig::IsInEditorMode())
		{
			if (Input::GetKey(SDL_SCANCODE_LCTRL) && Input::GetKeyDown(SDL_SCANCODE_S))
			{
				EventDispatcher::Notify(EventType::SaveScene, nullptr);
			}
		}

		ImGui_ImplSDL3_NewFrame();
		ImGui_ImplDX11_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();

		ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_NoUndocking;
		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport()->ID, nullptr, dockspaceFlags);

		// INFO: Update Editor Panels
		for (size_t i = 0; i < editorPanels.size(); i++)
		{
			std::unique_ptr<EditorPanel>& editorPanel = editorPanels[i];
			editorPanel->Update(deltaTime);
		}
	}

	void EditorRuntime::Release()
	{
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplSDL3_Shutdown();
		ImGui::DestroyContext();
	}

	void EditorRuntime::SetCustomStyle()
	{
		auto& colours = ImGui::GetStyle().Colors;
		colours[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };

		// INFO: Headers
		colours[ImGuiCol_Header] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colours[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colours[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// INFO: Buttons
		colours[ImGuiCol_Button] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colours[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colours[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// INFO: Frame Background
		colours[ImGuiCol_FrameBg] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colours[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colours[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// INFO: Tabs
		colours[ImGuiCol_Tab] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colours[ImGuiCol_TabHovered] = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
		colours[ImGuiCol_TabActive] = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
		colours[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colours[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };

		// INFO: Title
		colours[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colours[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colours[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// INFO: Checkbox
		colours[ImGuiCol_CheckMark] = ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f };

		// INFO: Sliders
		colours[ImGuiCol_SliderGrab] = ImVec4{ 0.4f, 0.405f, 0.41f, 1.0f };
		colours[ImGuiCol_SliderGrabActive] = ImVec4{ 0.5f, 0.505f, 0.51f, 1.0f };
	}
}
