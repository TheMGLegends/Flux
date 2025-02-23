#include <iostream>
#include <Windows.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_video.h>

#include <Mouse.h>

#include <nlohmann/json.hpp>

#include <assimp/cimport.h>
#include <assimp/postprocess.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <fmod.hpp>

#include <PxPhysicsAPI.h>

#include <imgui.h>
#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_dx11.h>
#include <d3d11.h>
#include <wrl.h>

#include <ImGuizmo.h>

#include "Application/Application.h"

int main()
{
	// INFO: SDL Integration Test
	/*SDL_InitFlags flags = SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC |
		SDL_INIT_HAPTIC | SDL_INIT_GAMEPAD | SDL_INIT_EVENTS | SDL_INIT_SENSOR | SDL_INIT_CAMERA;
	if (SDL_Init(flags))
	{
		std::cout << "Initialized SDL successfully!" << std::endl;
	}

	SDL_Quit();*/

	// INFO: DirectXTK Integration Test
	//DirectX::Mouse mouse;

	// INFO: JSON Integration Test
	/*nlohmann::ordered_json j = {
		{"pi", 3.141},
		{"happy", true},
		{"name", "Niels"},
		{"nothing", nullptr},
		{"answer", {
			{"everything", 42}
		}},
		{"list", {1, 0, 2}},
		{"object", {
			{"currency", "USD"},
			{"value", 42.99}
		}}
	};

	std::cout << j.dump(4) << std::endl;*/

	// INFO: Assimp Integration Test
	/*const aiScene* testImport = aiImportFile("Assets/Models/Cube.obj", aiProcessPreset_TargetRealtime_MaxQuality);

	if (testImport)
	{
		std::cout << "Imported model successfully!" << std::endl;
	}*/

	// INFO: FreeType Integration Test
	/*FT_Library library;
	if (FT_Init_FreeType(&library) == FT_Err_Ok)
	{
		std::cout << "Initialized FreeType successfully!" << std::endl;
	}*/

	// INFO: FMOD Integration Test
	/*CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

	FMOD_RESULT result;
	FMOD::System* system;

	result = FMOD::System_Create(&system);
	if (result == FMOD_OK)
	{
		std::cout << "Created FMOD system successfully!" << std::endl;
	}

	system->release();
	CoUninitialize();*/

	// INFO: PhysX Integration Test
	/*physx::PxDefaultAllocator allocator;
	physx::PxDefaultErrorCallback errorCallback;
	physx::PxFoundation* foundation = PxCreateFoundation(PX_PHYSICS_VERSION, allocator, errorCallback);

	physx::PxPvd* pvd = physx::PxCreatePvd(*foundation);
	physx::PxPhysics* physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, physx::PxTolerancesScale(), true, pvd);

	if (physics)
	{
		std::cout << "Created PhysX physics system successfully!" << std::endl;
	}

	physics->release();
	pvd->release();
	foundation->release();*/

	// INFO: Dear ImGui Integration Test
	/*SDL_Window* window = SDL_CreateWindow("Dear ImGui Test", 1280, 720, SDL_WINDOW_MAXIMIZED);

	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;

	HRESULT hResult = { S_OK };

	// INFO: Create the swap chain description
	DXGI_SWAP_CHAIN_DESC scd = { 0 };
	scd.BufferDesc.Width = 0;
	scd.BufferDesc.Height = 0;
	scd.BufferDesc.RefreshRate.Numerator = 0;
	scd.BufferDesc.RefreshRate.Denominator = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	scd.SampleDesc.Count = 1;
	scd.SampleDesc.Quality = 0;

	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.BufferCount = 1;
	scd.OutputWindow = (HWND)SDL_GetPointerProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr);
	scd.Windowed = TRUE;
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	// INFO: Create the device, device context, swap chain and front/back buffers
	hResult = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		D3D11_CREATE_DEVICE_DEBUG,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&scd,
		&swapChain,
		&device,
		nullptr,
		&deviceContext
	);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	ImGui::StyleColorsDark();

	if (ImGui_ImplSDL3_InitForD3D(window))
	{
		std::cout << "Initialized Dear ImGui SDL3 for D3D successfully!" << std::endl;
	}

	if (ImGui_ImplDX11_Init(device.Get(), deviceContext.Get()))
	{
		std::cout << "Initialized Dear ImGui successfully!" << std::endl;
	}

	(void)getchar();

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	SDL_DestroyWindow(window);*/

	// INFO: ImGuizmo Integration Test
	/*std::string isUsing = ImGuizmo::IsUsing() ? "true" : "false";
	std::cout << "IsUsing is " << isUsing << std::endl;*/

	Application app;
	app.Run();

	return 0;
}