#include "Renderer.h"

#include "../Globals.h"

using namespace Microsoft::WRL;
using namespace DirectX;

Renderer::Renderer() : device(nullptr), deviceContext(nullptr), swapChain(nullptr), renderTargetView(nullptr), 
					   depthStencilView(nullptr), spriteBatch(nullptr), viewport()
{
}

Renderer::~Renderer()
{
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
		// TODO: Logging System Log Error Message
		return hResult;
	}

	// INFO: Get the back buffer
	ComPtr<ID3D11Resource> backBuffer;

	hResult = swapChain->GetBuffer(0, __uuidof(ID3D11Resource), &backBuffer);

	if (FAILED(hResult))
	{
		// TODO: Logging System Log Error Message
		return hResult;
	}

	// INFO: Create the render target view
	hResult = device->CreateRenderTargetView(backBuffer.Get(), nullptr, &renderTargetView);

	if (FAILED(hResult))
	{
		// TODO: Logging System Log Error Message
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
		// TODO: Logging System Log Error Message
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
		// TODO: Logging System Log Error Message
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
		// TODO: Logging System Log Error Message
		return E_FAIL;
	}

	return hResult; // INFO: S_OK so long as we've made it this far
}

void Renderer::RenderFrame(Scene& scene)
{
	// TODO: Access currently active camera and set render target view colour to camera clear colour
	deviceContext->ClearRenderTargetView(renderTargetView.Get(), Colors::DarkSeaGreen);
	deviceContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// TODO: Ensure camera is valid

	// TODO: Get matrices (Translation, View, Projection)

	// TODO: Check if skybox game object has a skybox texture and is valid, if so render it otherwise use camera background colour

	// TODO: Go through all visualizer components and render them

	// TODO: Given we are in editor mode, go through all IDebugWireframe and render them

	spriteBatch->Begin();
	// TODO: Render all UI elements (FPS Counter)
	spriteBatch->End();

	swapChain->Present((UINT)RendererSettings::ENABLE_VSYNC, 0);
}
