#pragma once

#define NOMINMAX

#include <filesystem>
#include <memory>
#include <optional>
#include <unordered_map>
#include <Windows.h>

#include "ConstantBufferData.h"
#include "ShaderData.h"

#include "Core/Configs/DirectXConfig.h"
#include "Core/Renderer/Material.h"

namespace Assimp { class Importer; }
namespace DirectX { inline namespace DX11 { class SpriteFont; } }

struct ID3D11DepthStencilState;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11RasterizerState;
struct ID3D11SamplerState;
struct ID3D11ShaderResourceView;

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
		static HRESULT LoadTexture(const std::filesystem::path& texturePath, bool isDDS = false);
		static int LoadModel(const std::filesystem::path& modelPath, Assimp::Importer& importer);
		static int LoadAudio(const std::filesystem::path& audioPath);

		static int StoreScenePath(const std::string& sceneName, const std::filesystem::path& scenePath);

		static ShaderData& GetShaderData(DirectXConfig::ShaderType shaderType);
		static ConstantBufferData& GetConstantBufferData(DirectXConfig::ConstantBufferType constantBufferType);
		static ID3D11DepthStencilState* GetDepthWriteState(DirectXConfig::DepthWriteType depthWriteType);
		static ID3D11RasterizerState* GetCullingModeState(DirectXConfig::CullingModeType cullingModeType);
		static ID3D11SamplerState* GetSamplerState() { return samplerState.Get(); }
		static DirectX::SpriteFont* GetFont(const std::string& fontName);
		static ID3D11ShaderResourceView* GetTexture(const std::string& textureName, bool isSkyboxTexture = false);
		static bool HasTexture(const std::string& textureName, bool isSkyboxTexture = false);
		static Model* GetModel(const std::string& modelName);
		static bool HasModel(const std::string& modelName);

		/// @brief Returns a copy of a default material that can then be modified
		static Material GetMaterial(DirectXConfig::ShaderType shaderType);
		static const std::filesystem::path& GetAudioPath(const std::string& audioName);
		static bool HasAudioFile(const std::string& audioName);

		static std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>& GetTextures() { return textures; }
		static std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>& GetSkyboxTextures() { return skyboxTextures; }
		static std::unordered_map<std::string, std::unique_ptr<Model>>& GetModels() { return models; }

		static const std::filesystem::path& GetScenePath(const std::string& sceneName);
		static int GetSceneCount() { return static_cast<int>(scenePaths.size()); }

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
		static std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> skyboxTextures;
		static std::unordered_map<std::string, std::unique_ptr<Model>> models;
		static std::unordered_map<DirectXConfig::ShaderType, Material> materials;
		static std::unordered_map<std::string, std::filesystem::path> audioPaths;

		static std::unordered_map<std::string, std::filesystem::path> scenePaths;

		static ConstantBufferData EMPTY_CONSTANT_BUFFER_DATA;
		static ShaderData EMPTY_SHADER_DATA;
	};
}

