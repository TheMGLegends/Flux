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

		/// @param: _viewport The viewport to use for rendering (Scene View Panel Dimensions)
		HRESULT Initialise(HWND hWnd, const Viewport& _viewport);

		void RenderFrame(Scene& scene);

		inline ID3D11Device& GetDevice() { return *device.Get(); }
		inline ID3D11DeviceContext& GetDeviceContext() { return *deviceContext.Get(); }

	private:
		void OnWindowResized();

	private:
		Microsoft::WRL::ComPtr<ID3D11Device> device;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;
		Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;

		D3D11_VIEWPORT viewport;


		// INFO: UI Rendering

		std::unique_ptr<DirectX::SpriteBatch> spriteBatch;


		// INFO: Debug Wireframe Rendering

		std::unique_ptr<DirectX::BasicEffect> batchEffect;
		std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> primitiveBatch;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> batchInputLayout;
	};
}

