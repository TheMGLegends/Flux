#include "EditorRuntime.h"

#include <imgui.h>
#include <ImGuizmo.h>
#include <backends/imgui_impl_dx11.h>
#include <backends/imgui_impl_sdl3.h>

#include "Core/GlobalDefines.h"
#include "Core/Configs/EditorConfig.h"
#include "Core/Debug/Debug.h"
#include "Core/Debug/FrameRateMonitor.h"
#include "Core/Input/Input.h"
#include "Editor/Panels/SceneView.h"

using namespace Flux;
using namespace Flux::GlobalDefines;

EditorRuntime::EditorRuntime() : sceneView(nullptr)
{
}

EditorRuntime::~EditorRuntime()
{
}

void EditorRuntime::Release()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();
}

int EditorRuntime::PreInitialise(SDL_Window* window, ID3D11Device& device, ID3D11DeviceContext& deviceContext)
{
	// INFO: ImGui Initialisation
	IMGUI_CHECKVERSION();
	ImGuiContext* context = ImGui::CreateContext();

	// INFO: Set Flags for ImGui
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	// INFO: Set Custom ImGui Style
	SetCustomStyle();

	// INFO: Set Custom ImGui Font
	io.FontDefault = io.Fonts->AddFontFromFileTTF("Assets/Default/Fonts/OpenSans-Bold.ttf", EditorConfig::FONT_SIZE);

	// INFO: Setup ImGui Implementations for SDL and DirectX11
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

	if (!context)
	{
		Debug::LogError("EditorRuntime::PreInitialise() - Failed to create ImGui Context");
		return FLUX_FAILURE;
	}

	ImGuizmo::SetImGuiContext(context);

	return FLUX_SUCCESS;
}

int EditorRuntime::Initialise(Renderer& renderer)
{
	// INFO: Initialise Frame Rate Monitor
	FrameRateMonitor::Initialise();

	// INFO: Editor Panel Initialisation
	editorPanels.emplace_back(std::make_unique<SceneView>(renderer));
	sceneView = static_cast<SceneView*>(editorPanels.back().get());

	for (size_t i = 0; i < editorPanels.size(); i++) { editorPanels[i]->Initialise(); }

	// TODO: Initialisation Logic
	return FLUX_SUCCESS;
}

void EditorRuntime::Update(float deltaTime)
{
	if (Input::GetKeyDown(SDL_SCANCODE_F3)) { FrameRateMonitor::Toggle(); }

	FrameRateMonitor::Update(deltaTime);

	// INFO: ImGui New Frame
	ImGui_ImplSDL3_NewFrame();
	ImGui_ImplDX11_NewFrame();
	ImGui::NewFrame();
	ImGuizmo::BeginFrame();

	ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_NoUndocking;
	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport()->ID, nullptr, dockspaceFlags);

	// TODO: TESTING
	ImGui::Begin("Hierarchy");
	ImGui::Text("Hello, World!");
	ImGui::End();
	ImGui::Begin("Details");
	ImGui::Text("Hello, World!");
	ImGui::End();
	ImGui::Begin("Content Browser");
	ImGui::Text("Hello, World!");
	ImGui::End();

	// INFO: Update Editor Panels
	for (auto& editorPanel : editorPanels) { editorPanel->Update(deltaTime); }
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
}
