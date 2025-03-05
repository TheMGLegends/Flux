#pragma once

#include <d3d11.h>
#include <filesystem>
#include <optional>
#include <SpriteFont.h>
#include <unordered_map>
#include <Windows.h>

#include "ConstantBufferData.h"
#include "ShaderData.h"

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

		/// @brief Loads core assets as well as assets from the specified directory
		static HRESULT LoadAssets(const std::filesystem::path& assetDirectory);

	private:
		static std::optional<std::reference_wrapper<ID3D11Device>> device;
		static std::optional<std::reference_wrapper<ID3D11DeviceContext>> deviceContext;

		// TODO: Maybe key should be something else like filesystem::path or part of it
		static std::unordered_map<std::string, ShaderData> shaders;
	};
}

