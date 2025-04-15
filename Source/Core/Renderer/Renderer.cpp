#include "Renderer.h"

#include <Effects.h>
#include <imgui.h>
#include <SpriteBatch.h>
#include <backends/imgui_impl_dx11.h>

#include "ConstantBuffers.h"
#include "Material.h"

#include "Core/GlobalDefines.h"

#include "Core/Configs/DirectXConfig.h"
#include "Core/Configs/EditorConfig.h"
#include "Core/Configs/EngineConfig.h"
#include "Core/Configs/RendererConfig.h"
#include "Core/Configs/RuntimeConfig.h"

#include "Core/Debug/Debug.h"
#include "Core/Debug/FrameRateMonitor.h"
#include "Core/EventSystem/EventDispatcher.h"
#include "Core/Renderer/AssetHandler.h"

#include "Engine/Scene/Scene.h"

#include "Engine/Entities/GameObjects/GameObject.h"
#include "Engine/Entities/Components/Camera.h"
#include "Engine/Entities/Components/Transform.h"
#include "Engine/Entities/Components/Visualizer.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;
using namespace Microsoft::WRL;

namespace Flux
{
	using namespace ConstantBuffers;
	using namespace DirectXConfig;
	using namespace GlobalDefines;

	Renderer::Renderer() : device(nullptr), deviceContext(nullptr), swapChain(nullptr), renderTargetView(nullptr),
		depthStencilView(nullptr), spriteBatch(nullptr), backBufferViewport(), sceneViewViewport(),
		depthDisabled(nullptr)
	{
	}

	Renderer::~Renderer()
	{
	}

	void Renderer::OnNotify(EventType eventType, std::shared_ptr<Event> event)
	{
		if (eventType == EventType::WindowResized)
		{
			OnWindowResized();
		}
		else if (eventType == EventType::SceneViewResized)
		{
			OnSceneViewResized();
		}
	}

	HRESULT Renderer::Initialise(HWND hWnd)
	{
		HRESULT hResult = { S_OK };

		// INFO: Set the viewports
		backBufferViewport.Width = static_cast<float>(EngineConfig::GetWindowWidth());
		backBufferViewport.Height = static_cast<float>(EngineConfig::GetWindowHeight());
		backBufferViewport.MinDepth = 0.0f;
		backBufferViewport.MaxDepth = 1.0f;
		backBufferViewport.TopLeftX = 0.0f;
		backBufferViewport.TopLeftY = 0.0f;

		sceneViewViewport.Width = static_cast<float>(EditorConfig::GetSceneViewWidth());
		sceneViewViewport.Height = static_cast<float>(EditorConfig::GetSceneViewHeight());
		sceneViewViewport.MinDepth = 0.0f;
		sceneViewViewport.MaxDepth = 1.0f;
		sceneViewViewport.TopLeftX = 0.0f;
		sceneViewViewport.TopLeftY = 0.0f;

		// INFO: Create the swap chain description
		DXGI_SWAP_CHAIN_DESC scd = { 0 };
		scd.BufferDesc.Width = static_cast<UINT>(backBufferViewport.Width);
		scd.BufferDesc.Height = static_cast<UINT>(backBufferViewport.Height);
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

		// INFO: Create the texture description for the render texture & depth stencil buffer
		D3D11_TEXTURE2D_DESC td = { 0 };
		td.Width = static_cast<UINT>(sceneViewViewport.Width);
		td.Height = static_cast<UINT>(sceneViewViewport.Height);
		td.MipLevels = 1;
		td.ArraySize = 1;
		td.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

		td.SampleDesc.Count = scd.SampleDesc.Count;
		td.SampleDesc.Quality = scd.SampleDesc.Quality;

		td.Usage = D3D11_USAGE_DEFAULT;
		td.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		td.CPUAccessFlags = 0;
		td.MiscFlags = 0;

		// INFO: Instead of accessing back buffer, we create a new texture for the render target
		hResult = device->CreateTexture2D(&td, nullptr, &renderTexture);

		if (FAILED(hResult))
		{
			Debug::LogError("Renderer::Initialise() - Failed to create Render Texture");
			return hResult;
		}

		// INFO: Create the render target view for the render texture
		hResult = device->CreateRenderTargetView(renderTexture.Get(), nullptr, &renderTargetView);

		if (FAILED(hResult))
		{
			Debug::LogError("Renderer::Initialise() - Failed to create Render Target View");
			return hResult;
		}

		// INFO: Create the back buffer render target view for ImGui
		ComPtr<ID3D11Texture2D> backBuffer;

		hResult = swapChain->GetBuffer(0, __uuidof(ID3D11Resource), &backBuffer);

		if (FAILED(hResult))
		{
			Debug::LogError("Renderer::Initialise() - Failed to get Back Buffer");
			return hResult;
		}

		hResult = device->CreateRenderTargetView(backBuffer.Get(), nullptr, &backBufferRenderTargetView);

		if (FAILED(hResult))
		{
			Debug::LogError("Renderer::Initialise() - Failed to create Back Buffer Render Target View");
			return hResult;
		}

		// INFO: Create the shader resource view for the render texture
		D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
		ZeroMemory(&srvd, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		srvd.Format = td.Format;
		srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvd.Texture2D.MipLevels = 1;

		hResult = device->CreateShaderResourceView(renderTexture.Get(), &srvd, &renderTextureShaderResourceView);

		if (FAILED(hResult))
		{
			Debug::LogError("Renderer::Initialise() - Failed to create Render Texture Shader Resource View");
			return hResult;
		}

		// INFO: Adjust the texture description to work for the depth stencil buffer
		td.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		td.BindFlags = D3D11_BIND_DEPTH_STENCIL;

		// INFO: Create the depth stencil buffer
		ComPtr<ID3D11Texture2D> depthStencilBuffer;
		hResult = device->CreateTexture2D(&td, nullptr, &depthStencilBuffer);

		if (FAILED(hResult))
		{
			Debug::LogError("Renderer::Initialise() - Failed to create Depth Stencil Buffer");
			return hResult;
		}

		// INFO: Create the depth stencil view description
		D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
		ZeroMemory(&dsvd, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
		dsvd.Format = td.Format;
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

		// INFO: Set the viewport to scene view
		deviceContext->RSSetViewports(1, &sceneViewViewport);

		// INFO: Create the sprite batch
		spriteBatch = std::make_unique<SpriteBatch>(deviceContext.Get());

		if (!spriteBatch)
		{
			Debug::LogError("Renderer::Initialise() - Failed to create Sprite Batch");
			return E_FAIL;
		}

		if (!RuntimeConfig::IsStandalone())
		{
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
		}

		// INFO: Setup Events to Listen For
		if (IS_FAILURE(EventDispatcher::AddListener(EventType::WindowResized, this)))
		{
			Debug::LogError("Renderer::Initialise() - Failed to add Window Resized event listener");
			return FLUX_FAILURE;
		}

		if (!RuntimeConfig::IsStandalone())
		{
			if (IS_FAILURE(EventDispatcher::AddListener(EventType::SceneViewResized, this)))
			{
				Debug::LogError("Renderer::Initialise() - Failed to add Scene View Resized event listener");
				return FLUX_FAILURE;
			}
		}

		return hResult; // INFO: S_OK so long as we've made it this far
	}

	int Renderer::PostInitialise()
	{
		// INFO: Load Depth Disabled State
		depthDisabled = AssetHandler::GetDepthWriteState(DepthWriteType::Disabled);

		if (!depthDisabled)
		{
			Debug::LogError("Renderer::PostInitialise() - Failed to load Depth Stencil State");
			return FLUX_FAILURE;
		}

		return FLUX_SUCCESS;
	}

	void Renderer::RenderFrame(Scene& scene)
	{
		std::shared_ptr<Camera> camera = scene.GetCamera().lock();
		bool isStandalone = RuntimeConfig::IsStandalone();

		if (!camera)
		{
			Debug::LogError("Renderer::RenderFrame() - No Camera Found");
			return;
		}

		// INFO: Overload cameras visual changes (Texture & Colour) with first active play mode camera
		if (RuntimeConfig::IsInEditorMode())
		{
			std::shared_ptr<Camera> playModeCamera = scene.GetCamera(true).lock();

			if (playModeCamera)
			{
				camera->SetUseSkybox(playModeCamera->UseSkybox());
				if (camera->UseSkybox()) { camera->SetMaterialTexture(playModeCamera->GetSkyboxTextureName()); }
				camera->SetBackgroundColour(playModeCamera->GetBackgroundColour());
			}
		}

		if (!isStandalone)
		{
			// INFO: Set the render target to be the texture
			deviceContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());

			// INFO: Set the matching viewport
			deviceContext->RSSetViewports(1, &sceneViewViewport);
		}
		else
		{
			// INFO: Set the render target to be the back buffer
			deviceContext->OMSetRenderTargets(1, backBufferRenderTargetView.GetAddressOf(), depthStencilView.Get());

			// INFO: Set the matching viewport
			deviceContext->RSSetViewports(1, &backBufferViewport);
		}

		deviceContext->ClearRenderTargetView(renderTargetView.Get(), camera->GetBackgroundColour().data());
		deviceContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		Vector3 cameraPosition = camera->GetGameObject()->transform.expired() ? Vector3::Zero : camera->GetGameObject()->transform.lock()->GetPosition();

		XMMATRIX translation = XMMatrixTranslationFromVector(cameraPosition);
		XMMATRIX view = camera->GetViewMatrix();
		XMMATRIX projection = camera->GetProjectionMatrix();

		if (camera->UseSkybox()) { camera->DrawSkybox(*deviceContext.Get(), translation, view, projection); }

		for (auto& weakVisualizer : scene.GetComponents<Visualizer>())
		{
			std::shared_ptr<Visualizer> visualizer = weakVisualizer.lock();
			if (!visualizer) { continue; }

			GameObject* owningGameObject = visualizer->GetGameObject();
			if (!owningGameObject->IsActive() || !visualizer->IsActive()) { continue; }

			Material& material = visualizer->GetMaterial();
			std::shared_ptr<Transform> transform = owningGameObject->transform.lock();

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
		if (RuntimeConfig::IsInEditorMode() || RuntimeConfig::IsPaused())
		{

			batchEffect->SetView(view);
			batchEffect->SetProjection(projection);
			batchEffect->Apply(deviceContext.Get());

			// INFO: Disable Depth Testing
			deviceContext->OMSetDepthStencilState(depthDisabled, 0);

			deviceContext->IASetInputLayout(batchInputLayout.Get());

			primitiveBatch->Begin();
			scene.DrawWireframes(*deviceContext.Get(), *primitiveBatch.get());
			primitiveBatch->End();
		}

		// INFO: Render the UI
		spriteBatch->Begin();
		if (!isStandalone) { FrameRateMonitor::Render(spriteBatch.get()); }
		spriteBatch->End();

		if (!isStandalone)
		{
			// INFO: Set the render target to be the back buffer
			deviceContext->OMSetRenderTargets(1, backBufferRenderTargetView.GetAddressOf(), nullptr);

			// INFO: Set the matching viewport
			deviceContext->RSSetViewports(1, &backBufferViewport);

			// INFO: Render ImGui
			ImGui::Render(); // INFO: Render Main Viewport
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

			// INFO: Render Other Viewports
#ifdef _DEBUG
			if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			{
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
			}
#endif
		}

		swapChain->Present((UINT)RendererConfig::vsyncEnabled, 0);
	}

	void Renderer::OnWindowResized()
	{
		int windowWidth = EngineConfig::GetWindowWidth();
		int windowHeight = EngineConfig::GetWindowHeight();

		if (windowWidth == 0.0f || windowHeight == 0.0f) { return; }

		if (swapChain)
		{
			deviceContext->OMSetRenderTargets(0, 0, 0);

			backBufferRenderTargetView->Release();

			HRESULT hResult = { S_OK };

			hResult = swapChain->ResizeBuffers(0, windowWidth, windowHeight, DXGI_FORMAT_UNKNOWN, 0);

			if (FAILED(hResult))
			{
				Debug::LogError("Renderer::OnWindowResized() - Failed to resize buffers");
				return;
			}

			// INFO: Create the back buffer render target view for ImGui
			ComPtr<ID3D11Texture2D> backBuffer;

			hResult = swapChain->GetBuffer(0, __uuidof(ID3D11Resource), &backBuffer);

			if (FAILED(hResult))
			{
				Debug::LogError("Renderer::Initialise() - Failed to get Back Buffer");
				return;
			}

			hResult = device->CreateRenderTargetView(backBuffer.Get(), nullptr, backBufferRenderTargetView.GetAddressOf());

			if (FAILED(hResult))
			{
				Debug::LogError("Renderer::Initialise() - Failed to create Back Buffer Render Target View");
				return;
			}

			backBufferViewport.Width = static_cast<float>(windowWidth);
			backBufferViewport.Height = static_cast<float>(windowHeight);

			deviceContext->OMSetRenderTargets(1, backBufferRenderTargetView.GetAddressOf(), nullptr);
			deviceContext->RSSetViewports(1, &backBufferViewport);
		}
	}

	void Renderer::OnSceneViewResized()
	{
		float sceneViewWidth = EditorConfig::GetSceneViewWidth();
		float sceneViewHeight = EditorConfig::GetSceneViewHeight();

		if (sceneViewWidth < 1.0f || sceneViewHeight < 1.0f) { return; }

		if (renderTargetView)
		{
			deviceContext->OMSetRenderTargets(0, 0, 0);

			renderTexture.Reset();
			renderTextureShaderResourceView.Reset();
			renderTargetView.Reset();
			depthStencilView.Reset();

			HRESULT hResult = { S_OK };

			// INFO: Create the texture description for the render texture & depth stencil buffer
			D3D11_TEXTURE2D_DESC td = { 0 };
			td.Width = static_cast<UINT>(sceneViewWidth);
			td.Height = static_cast<UINT>(sceneViewHeight);
			td.MipLevels = 1;
			td.ArraySize = 1;
			td.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

			td.SampleDesc.Count = 1;
			td.SampleDesc.Quality = 0;

			td.Usage = D3D11_USAGE_DEFAULT;
			td.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
			td.CPUAccessFlags = 0;
			td.MiscFlags = 0;

			// INFO: Instead of accessing back buffer, we create a new texture for the render target
			hResult = device->CreateTexture2D(&td, nullptr, &renderTexture);

			if (FAILED(hResult))
			{
				Debug::LogError("Renderer::OnSceneViewResized() - Failed to create Render Texture");
				return;
			}

			// INFO: Create the render target view for the render texture
			hResult = device->CreateRenderTargetView(renderTexture.Get(), nullptr, &renderTargetView);

			if (FAILED(hResult))
			{
				Debug::LogError("Renderer::OnSceneViewResized() - Failed to create Render Target View");
				return;
			}

			// INFO: Create the shader resource view for the render texture
			D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
			ZeroMemory(&srvd, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
			srvd.Format = td.Format;
			srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvd.Texture2D.MipLevels = 1;

			hResult = device->CreateShaderResourceView(renderTexture.Get(), &srvd, &renderTextureShaderResourceView);

			if (FAILED(hResult))
			{
				Debug::LogError("Renderer::OnSceneViewResized() - Failed to create Render Texture Shader Resource View");
				return;
			}

			// INFO: Adjust the texture description to work for the depth stencil buffer
			td.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			td.BindFlags = D3D11_BIND_DEPTH_STENCIL;

			ComPtr<ID3D11Texture2D> depthStencilBuffer;
			hResult = device->CreateTexture2D(&td, nullptr, &depthStencilBuffer);

			if (FAILED(hResult))
			{
				Debug::LogError("Renderer::OnSceneViewResized() - Failed to create depth stencil buffer");
				return;
			}

			D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
			ZeroMemory(&dsvd, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
			dsvd.Format = td.Format;
			dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			dsvd.Flags = 0;

			hResult = device->CreateDepthStencilView(depthStencilBuffer.Get(), &dsvd, &depthStencilView);

			if (FAILED(hResult))
			{
				Debug::LogError("Renderer::OnSceneViewResized() - Failed to create depth stencil view");
				return;
			}

			sceneViewViewport.Width = sceneViewWidth;
			sceneViewViewport.Height = sceneViewHeight;

			deviceContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());
			deviceContext->RSSetViewports(1, &sceneViewViewport);
		}
	}
}
