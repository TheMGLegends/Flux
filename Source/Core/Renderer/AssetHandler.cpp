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
