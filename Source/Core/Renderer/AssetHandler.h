#pragma once

#include <d3d11.h>
#include <filesystem>
#include <memory>
#include <optional>
#include <SpriteFont.h>
#include <unordered_map>
#include <Windows.h>

#include "ConstantBufferData.h"
#include "ShaderData.h"
#include "Core/Configs/DirectXConfig.h"

namespace Assimp { class Importer; }

namespace Flux
{
	class Material;
	class Model;

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

		// TODO: Get rid of the FreeType font library and use SpriteFont instead
		static bool LoadFont(const std::filesystem::path& fontPath); // INFO: SpriteFont for UI text rendering (ImGui Fonts are not stored/loaded here)
		static HRESULT LoadTexture(const std::filesystem::path& texturePath);
		static bool LoadModel(const std::filesystem::path& modelPath, Assimp::Importer& importer);


		// INFO: Getters

		static inline ShaderData& GetShaderData(DirectXConfig::ShaderType shaderType) { return shaders[shaderType]; }
		static inline ConstantBufferData& GetConstantBufferData(DirectXConfig::ConstantBufferType constantBufferType) { return constantBuffers[constantBufferType]; }
		static inline ID3D11DepthStencilState* GetDepthWriteState(DirectXConfig::DepthWriteType depthWriteType) { return depthWriteStates[depthWriteType].Get(); }
		static inline ID3D11RasterizerState* GetCullingModeState(DirectXConfig::CullingModeType cullingModeType) { return cullingModeStates[cullingModeType].Get(); }
		static inline ID3D11SamplerState* GetSamplerState() { return samplerState.Get(); }
		static inline DirectX::SpriteFont* GetFont(const std::string& fontName) { return fonts[fontName].get(); }
		static inline ID3D11ShaderResourceView* GetTexture(const std::string& textureName) { return textures[textureName].Get(); }
		static Model* GetModel(const std::string& modelName);
		static Material* GetMaterial(DirectXConfig::ShaderType shaderType);

	private:
		static HRESULT LoadShaders(DirectXConfig::ShaderType shaderType, const std::filesystem::path& vertexShaderPath, const std::filesystem::path& pixelShaderPath);
		static HRESULT LoadConstantBuffer(DirectXConfig::ConstantBufferType constantBufferType);
		static HRESULT LoadDepthWriteState(DirectXConfig::DepthWriteType depthWriteType);
		static HRESULT LoadCullingModeState(DirectXConfig::CullingModeType cullingModeType);
		static HRESULT LoadSamplerState();
		static bool LoadMaterial(DirectXConfig::ShaderType shaderType);

		static HRESULT VerifyDeviceAndContext(bool verifyContext = false);

	private:
		static std::optional<std::reference_wrapper<ID3D11Device>> device;
		static std::optional<std::reference_wrapper<ID3D11DeviceContext>> deviceContext;

		static std::unordered_map<DirectXConfig::ShaderType, ShaderData> shaders;
		static std::unordered_map<DirectXConfig::ConstantBufferType, ConstantBufferData> constantBuffers;
		static std::unordered_map<DirectXConfig::DepthWriteType, Microsoft::WRL::ComPtr<ID3D11DepthStencilState>> depthWriteStates;
		static std::unordered_map<DirectXConfig::CullingModeType, Microsoft::WRL::ComPtr<ID3D11RasterizerState>> cullingModeStates;

		static Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;

		static std::unordered_map<std::string, std::unique_ptr<DirectX::SpriteFont>> fonts;
		static std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> textures;
		static std::unordered_map<std::string, std::unique_ptr<Model>> models;
		static std::unordered_map<DirectXConfig::ShaderType, std::unique_ptr<Material>> materials;
	};
}

