#include "Renderer.h"

#include <imgui.h>
#include <backends/imgui_impl_dx11.h>

#include "ConstantBuffers.h"
#include "Core/Configs/DirectXConfig.h"
#include "Core/Configs/EngineConfig.h"
#include "Core/Configs/RendererConfig.h"
#include "Core/Configs/RuntimeConfig.h"
#include "Core/Debug/Debug.h"
#include "Core/EventSystem/EventDispatcher.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Entities/GameObjects/GameObject.h"
#include "Engine/Entities/Components/Camera.h"
#include "Engine/Entities/Components/Transform.h"
#include "Engine/Entities/Components/Visualizer.h"
#include "Material.h"

using namespace DirectX;
using namespace Flux;
using namespace Flux::ConstantBuffers;
using namespace Flux::DirectXConfig;
using namespace DirectX::SimpleMath;
using namespace Microsoft::WRL;

Renderer::Renderer() : device(nullptr), deviceContext(nullptr), swapChain(nullptr), renderTargetView(nullptr), 
					   depthStencilView(nullptr), spriteBatch(nullptr), viewport()
{
}

Renderer::~Renderer()
{
}

void Renderer::OnNotify(EventType eventType, std::shared_ptr<Event> event)
{
	if (eventType == EventType::WindowResized)
		OnWindowResized();
}

HRESULT Renderer::Initialise(HWND hWnd, const Viewport& _viewport)
{
	HRESULT hResult = { S_OK };

	// INFO: Store the viewport
	viewport = _viewport;

	// INFO: Create the swap chain description
	DXGI_SWAP_CHAIN_DESC scd = { 0 };
	scd.BufferDesc.Width = static_cast<UINT>(viewport.Width);
	scd.BufferDesc.Height = static_cast<UINT>(viewport.Height);
	scd.BufferDesc.RefreshRate.Numerator = 0;
	scd.BufferDesc.RefreshRate.Denominator = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	scd.SampleDesc.Count = 1;
	scd.SampleDesc.Quality = 0;

	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.BufferCount = 1;
	scd.OutputWindow = hWnd;
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

	if (FAILED(hResult))
	{
		Debug::LogError("Renderer::Initialise() - Failed to create Device, Device Context and Swap Chain");
		return hResult;
	}

	// INFO: Get the back buffer
	ComPtr<ID3D11Resource> backBuffer;

	hResult = swapChain->GetBuffer(0, __uuidof(ID3D11Resource), &backBuffer);

	if (FAILED(hResult))
	{
		Debug::LogError("Renderer::Initialise() - Failed to get Back Buffer");
		return hResult;
	}

	// INFO: Create the render target view
	hResult = device->CreateRenderTargetView(backBuffer.Get(), nullptr, &renderTargetView);

	if (FAILED(hResult))
	{
		Debug::LogError("Renderer::Initialise() - Failed to create Render Target View");
		return hResult;
	}

	// INFO: Create the depth stencil buffer description
	D3D11_TEXTURE2D_DESC dsbd = { 0 };
	dsbd.Width = static_cast<UINT>(viewport.Width);
	dsbd.Height = static_cast<UINT>(viewport.Height);
	dsbd.MipLevels = 1;
	dsbd.ArraySize = 1;
	dsbd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	dsbd.SampleDesc.Count = scd.SampleDesc.Count;
	dsbd.SampleDesc.Quality = scd.SampleDesc.Quality;

	dsbd.Usage = D3D11_USAGE_DEFAULT;
	dsbd.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	dsbd.CPUAccessFlags = 0;
	dsbd.MiscFlags = 0;

	// INFO: Create the depth stencil buffer
	ComPtr<ID3D11Texture2D> depthStencilBuffer;
	hResult = device->CreateTexture2D(&dsbd, nullptr, &depthStencilBuffer);

	if (FAILED(hResult))
	{
		Debug::LogError("Renderer::Initialise() - Failed to create Depth Stencil Buffer");
		return hResult;
	}

	// INFO: Create the depth stencil view description
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
	ZeroMemory(&dsvd, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	dsvd.Format = dsbd.Format;
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvd.Flags = 0;

	// INFO: Create the depth stencil view
	hResult = device->CreateDepthStencilView(depthStencilBuffer.Get(), &dsvd, &depthStencilView);

	if (FAILED(hResult))
	{
		Debug::LogError("Renderer::Initialise() - Failed to create Depth Stencil View");
		return hResult;
	}

	// INFO: Set the render target view and depth stencil view
	deviceContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());

	// INFO: Set the viewport
	deviceContext->RSSetViewports(1, &viewport);

	// INFO: Create the sprite batch
	spriteBatch = std::make_unique<SpriteBatch>(deviceContext.Get());

	if (!spriteBatch)
	{
		Debug::LogError("Renderer::Initialise() - Failed to create Sprite Batch");
		return E_FAIL;
	}

	// INFO: Create the resources needed for the debug wireframe rendering
	batchEffect = std::make_unique<BasicEffect>(device.Get());

	if (!batchEffect)
	{
		Debug::LogError("Renderer::Initialise() - Failed to create Basic Effect");
		return E_FAIL;
	}

	batchEffect->SetVertexColorEnabled(true);

	primitiveBatch = std::make_unique<PrimitiveBatch<VertexPositionColor>>(deviceContext.Get());

	if (!primitiveBatch)
	{
		Debug::LogError("Renderer::Initialise() - Failed to create Primitive Batch");
		return E_FAIL;
	}

	const void* vertexShaderBytecode = nullptr;
	size_t vertexShaderBytecodeLength = 0;
	batchEffect->GetVertexShaderBytecode(&vertexShaderBytecode, &vertexShaderBytecodeLength);

	hResult = device->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, 
									    vertexShaderBytecode, vertexShaderBytecodeLength, &batchInputLayout);

	if (FAILED(hResult))
	{
		Debug::LogError("Renderer::Initialise() - Failed to create Batch Input Layout");
		return hResult;
	}

	// INFO: Setup Events to Listen For
	EventDispatcher::AddListener(EventType::WindowResized, this);

	return hResult; // INFO: S_OK so long as we've made it this far
}

void Renderer::RenderFrame(Scene& scene)
{
	std::shared_ptr<Camera> camera = scene.GetCamera().lock();

	if (!camera)
	{
		Debug::LogError("Renderer::RenderFrame() - No Camera Found");
		return;
	}

	deviceContext->ClearRenderTargetView(renderTargetView.Get(), camera->GetBackgroundColour().data());
	deviceContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	Vector3 cameraPosition = camera->GetGameObject()->transform.expired() ? Vector3::Zero : camera->GetGameObject()->transform.lock()->GetPosition();

	XMMATRIX translation = XMMatrixTranslationFromVector(cameraPosition);
	XMMATRIX view = camera->GetViewMatrix();
	XMMATRIX projection = camera->GetProjectionMatrix();

	if (camera->UseSkybox())
		camera->DrawSkybox(*deviceContext.Get(), translation, view, projection);

	for (auto& weakVisualizer : scene.GetComponents<Visualizer>())
	{
		if (weakVisualizer.expired())
			continue;

		std::shared_ptr<Visualizer> visualizer = weakVisualizer.lock();
		Material& material = visualizer->GetMaterial();
		GameObject* owningGameObject = visualizer->GetGameObject();
		std::shared_ptr<Transform> transform = owningGameObject->transform.lock();

		if (!visualizer->IsActive() || !owningGameObject->IsActive())
			continue;

		DirectX::XMMATRIX world = transform->GetWorldMatrix();

		switch (material.GetConstantBufferType())
		{
		case ConstantBufferType::Unlit:
		{
			UnlitVS unlitVS{};
			unlitVS.wvp = world * view * projection;
			deviceContext->UpdateSubresource(material.GetConstantBuffer(), 0, nullptr, &unlitVS, 0, 0);

			break;
		}
		case ConstantBufferType::None:
		default:
			break;
		}

		visualizer->Draw(*deviceContext.Get());
	}

	// INFO: Render the Debug Wireframes
	if (RuntimeConfig::IsInEditorMode())
	{
		batchEffect->SetView(view);
		batchEffect->SetProjection(projection);
		batchEffect->Apply(deviceContext.Get());

		deviceContext->IASetInputLayout(batchInputLayout.Get());

		primitiveBatch->Begin();
		scene.DrawWireframes(*deviceContext.Get(), *primitiveBatch.get());
		primitiveBatch->End();
	}

	// INFO: Render the UI
	spriteBatch->Begin();
	// TODO: Render all UI elements (FPS Counter)
	spriteBatch->End();

	// INFO: Render ImGui
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	swapChain->Present((UINT)RendererConfig::vsyncEnabled, 0);
}

void Renderer::OnWindowResized()
{
	if (EngineConfig::windowWidth == 0 || EngineConfig::windowHeight == 0)
		return;

	if (swapChain)
	{
		deviceContext->OMSetRenderTargets(0, 0, 0);

		renderTargetView->Release();
		depthStencilView->Release();

		HRESULT hResult = { S_OK };

		hResult = swapChain->ResizeBuffers(0, EngineConfig::windowWidth, EngineConfig::windowHeight, DXGI_FORMAT_UNKNOWN, 0);

		if (FAILED(hResult))
		{
			Debug::LogError("Renderer::OnWindowResized() - Failed to resize buffers");
			return;
		}

		ComPtr<ID3D11Resource> backBuffer;

		hResult = swapChain->GetBuffer(0, __uuidof(ID3D11Resource), &backBuffer);

		if (FAILED(hResult))
		{
			Debug::LogError("Renderer::OnWindowResized() - Failed to get back buffer");
			return;
		}

		hResult = device->CreateRenderTargetView(backBuffer.Get(), nullptr, renderTargetView.GetAddressOf());

		if (FAILED(hResult))
		{
			Debug::LogError("Renderer::OnWindowResized() - Failed to create render target view");
			return;
		}

		D3D11_TEXTURE2D_DESC dsb = { 0 };
		dsb.Width = EngineConfig::windowWidth;
		dsb.Height = EngineConfig::windowHeight;
		dsb.MipLevels = 1;
		dsb.ArraySize = 1;
		dsb.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

		dsb.SampleDesc.Count = 1;
		dsb.SampleDesc.Quality = 0;

		dsb.Usage = D3D11_USAGE_DEFAULT;
		dsb.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		dsb.CPUAccessFlags = 0;
		dsb.MiscFlags = 0;

		ComPtr<ID3D11Texture2D> depthStencilBuffer;
		hResult = device->CreateTexture2D(&dsb, nullptr, &depthStencilBuffer);

		if (FAILED(hResult))
		{
			Debug::LogError("Renderer::OnWindowResized() - Failed to create depth stencil buffer");
			return;
		}

		D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
		ZeroMemory(&dsvd, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
		dsvd.Format = dsb.Format;
		dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsvd.Flags = 0;

		hResult = device->CreateDepthStencilView(depthStencilBuffer.Get(), &dsvd, depthStencilView.GetAddressOf());

		if (FAILED(hResult))
		{
			Debug::LogError("Renderer::OnWindowResized() - Failed to create depth stencil view");
			return;
		}

		deviceContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());

		viewport.Width = static_cast<float>(EngineConfig::windowWidth);
		viewport.Height = static_cast<float>(EngineConfig::windowHeight);

		deviceContext->RSSetViewports(1, &viewport);
	}
}
