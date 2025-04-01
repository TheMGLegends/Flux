#pragma once

#define NOMINMAX
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
#include "Core/Renderer/Material.h"

namespace Assimp { class Importer; }

namespace Flux
{
	class Model;

	class AssetHandler
	{
	public:
		AssetHandler() = delete;
		~AssetHandler() = delete;
		AssetHandler(const AssetHandler&) = delete;
		AssetHandler& operator=(const AssetHandler&) = delete;

		/// @brief Initialises all core assets
		static HRESULT Initialise(ID3D11Device& _device, ID3D11DeviceContext& _deviceContext);

		/// @brief Loads assets from the specified directory
		static HRESULT LoadAssets(const std::filesystem::path& assetDirectory);

		static int LoadFont(const std::filesystem::path& fontPath); // INFO: SpriteFont for UI text rendering (ImGui Fonts are not stored/loaded here)
		static HRESULT LoadTexture(const std::filesystem::path& texturePath);
		static int LoadModel(const std::filesystem::path& modelPath, Assimp::Importer& importer);
		static int LoadAudio(const std::filesystem::path& audioPath);


		// INFO: Getters

		static ShaderData& GetShaderData(DirectXConfig::ShaderType shaderType);
		static ConstantBufferData& GetConstantBufferData(DirectXConfig::ConstantBufferType constantBufferType);
		static ID3D11DepthStencilState* GetDepthWriteState(DirectXConfig::DepthWriteType depthWriteType);
		static ID3D11RasterizerState* GetCullingModeState(DirectXConfig::CullingModeType cullingModeType);
		static inline ID3D11SamplerState* GetSamplerState() { return samplerState.Get(); }
		static DirectX::SpriteFont* GetFont(const std::string& fontName);
		static ID3D11ShaderResourceView* GetTexture(const std::string& textureName);
		static Model* GetModel(const std::string& modelName);

		/// @brief Returns a copy of a default material that can then be modified
		static Material GetMaterial(DirectXConfig::ShaderType shaderType);
		static const std::filesystem::path& GetAudioPath(const std::string& audioName);

	private:
		static HRESULT LoadShaders(DirectXConfig::ShaderType shaderType, const std::filesystem::path& vertexShaderPath, const std::filesystem::path& pixelShaderPath);
		static HRESULT LoadConstantBuffer(DirectXConfig::ConstantBufferType constantBufferType);
		static HRESULT LoadDepthWriteState(DirectXConfig::DepthWriteType depthWriteType);
		static HRESULT LoadCullingModeState(DirectXConfig::CullingModeType cullingModeType);
		static HRESULT LoadSamplerState();
		static int LoadMaterial(DirectXConfig::ShaderType shaderType);

		static HRESULT VerifyDeviceAndContext(bool verifyContext = false);

	public:
		static const std::filesystem::path EMPTY_PATH;

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
		static std::unordered_map<DirectXConfig::ShaderType, Material> materials;
		static std::unordered_map<std::string, std::filesystem::path> audioPaths;
	};
}

