#pragma once

#include <d3d11.h>
#include <optional>
#include <unordered_map>
#include <Windows.h>

namespace Flux
{
	class AssetHandler
	{
	public:
		AssetHandler() = delete;
		~AssetHandler() = delete;
		AssetHandler(const AssetHandler&) = delete;
		AssetHandler& operator=(const AssetHandler&) = delete;

		static HRESULT Initialise(ID3D11Device& _device, ID3D11DeviceContext& _deviceContext);

	private:
		static std::optional<std::reference_wrapper<ID3D11Device>> device;
		static std::optional<std::reference_wrapper<ID3D11DeviceContext>> deviceContext;


	};
}

