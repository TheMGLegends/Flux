#pragma once

#include "Core/EventSystem/IEventListener.h"

#define NOMINMAX

#include <d3d11.h>
#include <memory>
#include <PrimitiveBatch.h>
#include <VertexTypes.h>
#include <Windows.h>
#include <wrl.h>

namespace DirectX
{
	inline namespace DX11
	{
		class BasicEffect;
		class SpriteBatch;
	}
}

namespace Flux
{
	class Scene;

	class Renderer : public IEventListener
	{
	public:
		Renderer();
		~Renderer() override;

		void OnNotify(EventType eventType, std::shared_ptr<Event> event) override;

		HRESULT Initialise(HWND hWnd);
		int PostInitialise();
		void RenderFrame(Scene& scene);

		ID3D11Device& GetDevice();
		ID3D11DeviceContext& GetDeviceContext();
		ID3D11ShaderResourceView* GetRenderTextureShaderResourceView();

	private:
		/// @brief Resizes the buffer responsible for ImGui rendering
		void OnWindowResized();

		/// @brief Resizes the buffer responsible for Scene rendering
		void OnSceneViewResized();

	private:
		Microsoft::WRL::ComPtr<ID3D11Device> device;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;
		Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> renderTexture; // INFO: Acts as our back buffer
		Microsoft::WRL::ComPtr<ID3D11Texture2D> resolvedRenderTexture; // INFO: Used for MSAA
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> backBufferRenderTargetView;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> renderTextureShaderResourceView; // INFO: Used by ImGui to read the texture
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;

		D3D11_VIEWPORT backBufferViewport;
		D3D11_VIEWPORT sceneViewViewport;

		UINT sampleCount;
		UINT sampleQuality;


		// INFO: UI Rendering

		std::unique_ptr<DirectX::SpriteBatch> spriteBatch;


		// INFO: Debug Wireframe Rendering

		ID3D11DepthStencilState* depthDisabled;
		std::unique_ptr<DirectX::BasicEffect> batchEffect;
		std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> primitiveBatch;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> batchInputLayout;
	};
}

