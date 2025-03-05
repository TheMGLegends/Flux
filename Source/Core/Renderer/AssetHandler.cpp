#include "AssetHandler.h"

using namespace Flux;

std::optional<std::reference_wrapper<ID3D11Device>> AssetHandler::device;
std::optional<std::reference_wrapper<ID3D11DeviceContext>> AssetHandler::deviceContext;

HRESULT AssetHandler::Initialise(ID3D11Device& _device, ID3D11DeviceContext& _deviceContext)
{
	device = _device;
	deviceContext = _deviceContext;



	return S_OK;
}

HRESULT Flux::AssetHandler::LoadAssets(const std::filesystem::path& assetDirectory)
{
	// TODO: Load Required Assets (Shaders, Culling Modes, Depth Writes etc.)

	// TODO: Load Assets from the specified directory (Textures, Models etc.)

	return S_OK;
}
