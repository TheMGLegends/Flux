#include "EditorRuntime.h"

#include <imgui.h>
#include <backends/imgui_impl_dx11.h>
#include <backends/imgui_impl_sdl3.h>

#include "Core/Debug/Debug.h"

using namespace Flux;

EditorRuntime::EditorRuntime()
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

bool EditorRuntime::PreInitialise(SDL_Window* window, ID3D11Device& device, ID3D11DeviceContext& deviceContext)
{
	// INFO: ImGui Initialisation
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// INFO: Set Flags for ImGui
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	// INFO: Setup ImGui Implementations for SDL and DirectX11
	if (!ImGui_ImplSDL3_InitForD3D(window))
	{
		Debug::LogError("EditorRuntime::PreInitialise() - Failed to initialise ImGui SDL3 Implementation");
		return false;
	}

	if (!ImGui_ImplDX11_Init(&device, &deviceContext))
	{
		Debug::LogError("EditorRuntime::PreInitialise() - Failed to initialise ImGui DirectX11 Implementation");
		return false;
	}

	return true; // INFO: Pre-Initialisation Successful
}

bool EditorRuntime::Initialise()
{
	// TODO: Initialisation Logic

	return true; // INFO: Initialisation Successful
}

void EditorRuntime::Update(float deltaTime)
{
	// INFO: ImGui New Frame
	ImGui_ImplSDL3_NewFrame();
	ImGui_ImplDX11_NewFrame();
	ImGui::NewFrame();

	// TODO: TESTING
	ImGui::ShowDemoWindow();

	// TODO: Update Logic
}
