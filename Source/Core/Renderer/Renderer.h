#pragma once

#include "Core/EventSystem/IEventListener.h"

#include <d3d11.h>
#include <Effects.h>
#include <memory>
#include <PrimitiveBatch.h>
#include <SpriteBatch.h>
#include <VertexTypes.h>
#include <Windows.h>
#include <wrl.h>

namespace Flux
{
	class Scene;

	/// @brief Initializer for D3D11_VIEWPORT struct
	struct Viewport : D3D11_VIEWPORT
	{
		Viewport(float topLeftX, float topLeftY, float width, float height, float minDepth, float maxDepth)
		{
			TopLeftX = topLeftX;
			TopLeftY = topLeftY;
			Width = width;
			Height = height;
			MinDepth = minDepth;
			MaxDepth = maxDepth;
		}
	};

	class Renderer : public IEventListener
	{
	public:
		Renderer();
		~Renderer();

		virtual void OnNotify(EventType eventType, std::shared_ptr<Event> event) override;

		HRESULT Initialise(HWND hWnd);

		/// @return 0 if successful
		int PostInitialise();

		void RenderFrame(Scene& scene);

		inline ID3D11Device& GetDevice() { return *device.Get(); }
		inline ID3D11DeviceContext& GetDeviceContext() { return *deviceContext.Get(); }
		inline ID3D11ShaderResourceView* GetRenderTextureShaderResourceView() { return renderTextureShaderResourceView.Get(); }

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
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> backBufferRenderTargetView;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> renderTextureShaderResourceView; // INFO: Used by ImGui to read the texture
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;

		D3D11_VIEWPORT backBufferViewport;
		D3D11_VIEWPORT sceneViewViewport;

		// INFO: UI Rendering

		std::unique_ptr<DirectX::SpriteBatch> spriteBatch;


		// INFO: Debug Wireframe Rendering

		ID3D11DepthStencilState* depthDisabled;
		std::unique_ptr<DirectX::BasicEffect> batchEffect;
		std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> primitiveBatch;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> batchInputLayout;
	};
}

