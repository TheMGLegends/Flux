#pragma once

#include <d3d11.h>
#include <memory>
#include <SpriteBatch.h>
#include <Windows.h>
#include <wrl.h>

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

class Scene;

// TODO: Potentially make renderer listener for window resize events

class Renderer
{
public:
	Renderer();
	~Renderer();

	/// @param: _viewport The viewport to use for rendering (Scene View Panel Dimensions)
	HRESULT Initialise(HWND hWnd, const Viewport& _viewport);

	void RenderFrame(Scene& scene);

	inline ID3D11Device& GetDevice() { return *device.Get(); }
	inline ID3D11DeviceContext& GetDeviceContext() { return *deviceContext.Get(); }

private:
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;

	D3D11_VIEWPORT viewport;

	std::unique_ptr<DirectX::SpriteBatch> spriteBatch;
};

