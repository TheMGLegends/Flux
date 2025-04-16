#include "AssetHandler.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <DDSTextureLoader.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3d11shader.h>
#include <fmod.hpp>
#include <fmod_errors.h>
#include <imgui.h>
#include <magic_enum.hpp>
#include <SpriteFont.h>
#include <WICTextureLoader.h>

#include "ConstantBuffers.h"
#include "Material.h"
#include "Model.h"
#include "ReadData.h"

#include "Core/GlobalDefines.h"
#include "Core/Configs/FiletypeConfig.h"
#include "Core/Debug/Debug.h"
#include "Core/Helpers/StringHelpers.h"

using namespace Microsoft::WRL;

namespace Flux
{
	using namespace DirectXConfig;
	using namespace GlobalDefines;

	const std::filesystem::path AssetHandler::EMPTY_PATH = std::filesystem::path();

	std::optional<std::reference_wrapper<ID3D11Device>> AssetHandler::device;
	std::optional<std::reference_wrapper<ID3D11DeviceContext>> AssetHandler::deviceContext;

	std::unordered_map<ShaderType, ShaderData> AssetHandler::shaders;
	std::unordered_map<ConstantBufferType, ConstantBufferData> AssetHandler::constantBuffers;
	std::unordered_map<DepthWriteType, ComPtr<ID3D11DepthStencilState>> AssetHandler::depthWriteStates;
	std::unordered_map<CullingModeType, ComPtr<ID3D11RasterizerState>> AssetHandler::cullingModeStates;

	ComPtr<ID3D11SamplerState> AssetHandler::samplerState;

	std::unordered_map<std::string, std::unique_ptr<DirectX::SpriteFont>> AssetHandler::fonts;
	std::unordered_map<std::string, ImFont*> AssetHandler::imGuiFonts;
	std::unordered_map<std::string, ComPtr<ID3D11ShaderResourceView>> AssetHandler::textures;
	std::unordered_map<std::string, ComPtr<ID3D11ShaderResourceView>> AssetHandler::skyboxTextures;
	std::unordered_map<std::string, std::unique_ptr<Model>> AssetHandler::models;
	std::unordered_map<ShaderType, Material> AssetHandler::materials;
	std::unordered_map<std::string, std::filesystem::path> AssetHandler::audioPaths;

	std::unordered_map<std::string, std::filesystem::path> AssetHandler::scenePaths;

	ConstantBufferData AssetHandler::EMPTY_CONSTANT_BUFFER_DATA;
	ShaderData AssetHandler::EMPTY_SHADER_DATA;

	HRESULT AssetHandler::Initialise(ID3D11Device& _device, ID3D11DeviceContext& _deviceContext)
	{
		device = _device;
		deviceContext = _deviceContext;

		// INFO: Load Default Assets
		if (FAILED(LoadShaders(ShaderType::Unlit, "CompiledShaders/UnlitVertexShader.cso", "CompiledShaders/UnlitPixelShader.cso"))) { return E_FAIL; }
		if (FAILED(LoadShaders(ShaderType::Skybox, "CompiledShaders/SkyboxVertexShader.cso", "CompiledShaders/SkyboxPixelShader.cso"))) { return E_FAIL; }

		if (FAILED(LoadConstantBuffer(ConstantBufferType::Unlit))) { return E_FAIL; }

		if (FAILED(LoadDepthWriteState(DepthWriteType::Enabled))) { return E_FAIL; }
		if (FAILED(LoadDepthWriteState(DepthWriteType::Disabled))) { return E_FAIL; }

		if (FAILED(LoadCullingModeState(CullingModeType::FrontSolid))) { return E_FAIL; }
		if (FAILED(LoadCullingModeState(CullingModeType::BackSolid))) { return E_FAIL; }

		if (FAILED(LoadSamplerState())) { return E_FAIL; }

		if (FAILED(LoadAssets(FiletypeConfig::ASSET_DIRECTORY))) { return E_FAIL; }

		if (FLUX_FAIL(LoadMaterial(ShaderType::Unlit))) { return E_FAIL; }
		if (FLUX_FAIL(LoadMaterial(ShaderType::Skybox))) { return E_FAIL; }

		return S_OK;
	}

	HRESULT AssetHandler::LoadAssets(const std::filesystem::path& assetDirectory)
	{
		Assimp::Importer importer;

		// INFO: Go through all files and subdirectories in the specified directory and load assets
		for (const auto& entry : std::filesystem::recursive_directory_iterator(assetDirectory))
		{
			// INFO: Don't bother with checking directories (Folders)
			if (!entry.is_regular_file()) { continue; }

			std::string extensionType = StringHelpers::ToLower(entry.path().extension().string());

			// INFO: Font Loading
			if (extensionType == FiletypeConfig::SPRITEFONT && FLUX_FAIL(LoadFont(entry.path()))) { return E_FAIL; }

			// INFO: Skybox Texture Loading
			else if (extensionType == FiletypeConfig::DDS && FAILED(LoadTexture(entry.path(), true))) { return E_FAIL; }

			// INFO: Texture Loading
			else if (FiletypeConfig::IsSupportedTextureFormat(extensionType) && FAILED(LoadTexture(entry.path()))) { return E_FAIL; }

			// INFO: Model Loading
			else if (FiletypeConfig::IsSupportedModelFormat(extensionType) && FLUX_FAIL(LoadModel(entry.path(), importer))) { return E_FAIL; }

			// INFO: Audio Loading
			else if (FiletypeConfig::IsSupportedAudioFormat(extensionType) && FLUX_FAIL(StoreAudioPath(entry.path()))) { return E_FAIL; }

			// INFO: Scene Loading
			else if (extensionType == FiletypeConfig::SCENE && FLUX_FAIL(StoreScenePath(entry.path().stem().string(), entry.path()))) { return E_FAIL; }
		}

		return S_OK;
	}

	int AssetHandler::LoadFont(const std::filesystem::path& fontPath)
	{
		if (FAILED(VerifyDeviceAndContext()))
		{
			Debug::LogError("AssetHandler::LoadFont() - Unable to load font, see above error codes");
			return FLUX_FAILURE;
		}

		ID3D11Device& deviceRef = device->get();

		auto font = std::make_unique<DirectX::SpriteFont>(&deviceRef, fontPath.c_str());

		if (!font)
		{
			Debug::LogError("AssetHandler::LoadFont() - Failed to create font. Filepath: " + fontPath.string());
			return FLUX_FAILURE;
		}

		if (!fonts.try_emplace(fontPath.stem().string(), std::move(font)).second)
		{
			Debug::LogError("AssetHandler::LoadFont() - Failed to insert font into map. Filepath: " + fontPath.string());
			return FLUX_FAILURE;
		}

		return FLUX_SUCCESS;
	}

	int AssetHandler::LoadImGuiFont(const std::filesystem::path& fontPath, float pixelSize)
	{
		ImFont* font = ImGui::GetIO().Fonts->AddFontFromFileTTF(fontPath.string().c_str(), pixelSize);

		if (!font)
		{
			Debug::LogError("AssetHandler::LoadImGuiFont() - Failed to create ImGui font. Filepath: " + fontPath.string());
			return FLUX_FAILURE;
		}

		if (!imGuiFonts.try_emplace(fontPath.stem().string(), font).second)
		{
			Debug::LogError("AssetHandler::LoadImGuiFont() - Failed to insert ImGui font into map. Filepath: " + fontPath.string());
			return FLUX_FAILURE;
		}

		return FLUX_SUCCESS;
	}

	HRESULT AssetHandler::LoadTexture(const std::filesystem::path& texturePath, bool isDDS)
	{
		HRESULT hResult = S_OK;

		hResult = VerifyDeviceAndContext(true);

		if (FAILED(hResult))
		{
			Debug::LogError("AssetHandler::LoadTexture() - Unable to load texture, see above error codes");
			return hResult;
		}

		ID3D11Device& deviceRef = device->get();
		ID3D11DeviceContext& deviceContextRef = deviceContext->get();

		ComPtr<ID3D11ShaderResourceView> texture;

		// INFO: DDS Mainly used for Skybox Textures
		if (isDDS)
		{
			hResult = DirectX::CreateDDSTextureFromFile(&deviceRef, &deviceContextRef, texturePath.c_str(), nullptr, &texture);

			if (FAILED(hResult))
			{
				Debug::LogError("AssetHandler::LoadTexture() - Failed to create texture. Filepath: " + texturePath.string());
				return hResult;
			}

			if (!skyboxTextures.try_emplace(texturePath.stem().string(), texture).second)
			{
				Debug::LogError("AssetHandler::LoadTexture() - Failed to insert skybox texture into map. Filepath: " + texturePath.string());
				return E_FAIL;
			}
		}
		else
		{
			hResult = DirectX::CreateWICTextureFromFile(&deviceRef, &deviceContextRef, texturePath.c_str(), nullptr, &texture);

			if (FAILED(hResult))
			{
				Debug::LogError("AssetHandler::LoadTexture() - Failed to create texture. Filepath: " + texturePath.string());
				return hResult;
			}

			if (!textures.try_emplace(texturePath.stem().string(), std::move(texture)).second)
			{
				Debug::LogError("AssetHandler::LoadTexture() - Failed to insert texture into map. Filepath: " + texturePath.string());
				return E_FAIL;
			}
		}

		return S_OK;
	}

	int AssetHandler::LoadModel(const std::filesystem::path& modelPath, Assimp::Importer& importer)
	{
		if (FAILED(VerifyDeviceAndContext(true)))
		{
			Debug::LogError("AssetHandler::LoadModel() - Unable to load model, see above error codes");
			return FLUX_FAILURE;
		}

		ID3D11Device& deviceRef = device->get();
		ID3D11DeviceContext& deviceContextRef = deviceContext->get();

		const aiScene* scene = importer.ReadFile(modelPath.string(), aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_ConvertToLeftHanded);

		if (!scene)
		{
			Debug::LogError("AssetHandler::LoadModel() - Failed to load model. Filepath: " + modelPath.string());
			return FLUX_FAILURE;
		}

		auto model = std::make_unique<Model>(deviceRef, deviceContextRef, scene, modelPath.stem().string());

		if (!model)
		{
			Debug::LogError("AssetHandler::LoadModel() - Failed to create model. Filepath: " + modelPath.string());
			return FLUX_FAILURE;
		}

		if (!models.try_emplace(modelPath.stem().string(), std::move(model)).second)
		{
			Debug::LogError("AssetHandler::LoadModel() - Failed to insert model into map. Filepath: " + modelPath.string());
			return FLUX_FAILURE;
		}

		return FLUX_SUCCESS;
	}

	int AssetHandler::StoreAudioPath(const std::filesystem::path& audioPath)
	{
		if (!audioPaths.try_emplace(audioPath.stem().string(), audioPath).second)
		{
			Debug::LogError("AssetHandler::StoreAudioPath() - Failed to insert audio path into map. Filepath: " + audioPath.string());
			return FLUX_FAILURE;
		}

		return FLUX_SUCCESS;
	}

	int AssetHandler::StoreScenePath(const std::string& sceneName, const std::filesystem::path& scenePath)
	{
		if (!scenePaths.try_emplace(sceneName, scenePath).second)
		{
			Debug::LogError("AssetHandler::LoadAssets() - Failed to insert scene path into map. Filepath: " + scenePath.stem().string());
			return FLUX_FAILURE;
		}

		return FLUX_SUCCESS;
	}

	ShaderData& AssetHandler::GetShaderData(ShaderType shaderType)
	{
		if (auto it = shaders.find(shaderType); it != shaders.end()) { return it->second; }

		Debug::LogError(std::format("AssetHandler::GetShaderData() - Failed to find shader data. Shader Type: {}", magic_enum::enum_integer(shaderType)));
		return EMPTY_SHADER_DATA;
	}

	ConstantBufferData& AssetHandler::GetConstantBufferData(ConstantBufferType constantBufferType)
	{
		if (auto it = constantBuffers.find(constantBufferType); it != constantBuffers.end()) { return it->second; }

		Debug::LogError(std::format("AssetHandler::GetConstantBufferData() - Failed to find constant buffer data. Constant Buffer Type: {}", magic_enum::enum_integer(constantBufferType)));
		return EMPTY_CONSTANT_BUFFER_DATA;
	}

	ID3D11DepthStencilState* AssetHandler::GetDepthWriteState(DepthWriteType depthWriteType)
	{
		if (auto it = depthWriteStates.find(depthWriteType); it != depthWriteStates.end()) { return it->second.Get(); }

		Debug::LogError(std::format("AssetHandler::GetDepthWriteState() - Failed to find depth write state. Depth Write Type: {}", magic_enum::enum_integer(depthWriteType)));
		return nullptr;
	}

	ID3D11RasterizerState* AssetHandler::GetCullingModeState(CullingModeType cullingModeType)
	{
		if (auto it = cullingModeStates.find(cullingModeType); it != cullingModeStates.end()) { return it->second.Get(); }

		Debug::LogError(std::format("AssetHandler::GetCullingModeState() - Failed to find culling mode state. Culling Mode Type: {}", magic_enum::enum_integer(cullingModeType)));
		return nullptr;
	}

	ID3D11SamplerState* AssetHandler::GetSamplerState()
	{
		return samplerState.Get();
	}

	DirectX::SpriteFont* AssetHandler::GetFont(const std::string& fontName)
	{
		if (auto it = fonts.find(fontName); it != fonts.end()) { return it->second.get(); }

		Debug::LogError("AssetHandler::GetFont() - Failed to find font. Font Name: " + fontName);
		return nullptr;
	}

	ImFont* AssetHandler::GetImGuiFont(const std::string& fontName)
	{
		if (auto it = imGuiFonts.find(fontName); it != imGuiFonts.end()) { return it->second; }

		Debug::LogError("AssetHandler::GetImGuiFont() - Failed to find ImGui font. Font Name: " + fontName);
		return nullptr;
	}

	ID3D11ShaderResourceView* AssetHandler::GetTexture(const std::string& textureName, bool isSkyboxTexture)
	{
		if (isSkyboxTexture)
		{
			auto it = skyboxTextures.find(textureName);
			if (it != skyboxTextures.end()) { return it->second.Get(); }
		}
		else
		{
			auto it = textures.find(textureName);
			if (it != textures.end()) { return it->second.Get(); }
		}

		Debug::LogError("AssetHandler::GetTexture() - Failed to find texture. Texture Name: " + textureName);
		return nullptr;
	}

	bool AssetHandler::HasTexture(const std::string& textureName, bool isSkyboxTexture)
	{
		if (isSkyboxTexture)
		{
			return skyboxTextures.contains(textureName);
		}
		else
		{
			return textures.contains(textureName);
		}
	}

	Model* AssetHandler::GetModel(const std::string& modelName)
	{
		if (auto it = models.find(modelName); it != models.end()) { return it->second.get(); }

		Debug::LogError("AssetHandler::GetModel() - Failed to find model. Model Name: " + modelName);
		return nullptr;
	}

	bool AssetHandler::HasModel(const std::string& modelName)
	{
		return models.contains(modelName);
	}

	Material AssetHandler::GetMaterial(ShaderType shaderType)
	{
		if (auto it = materials.find(shaderType); it != materials.end()) { return it->second; }

		Debug::LogError("AssetHandler::GetMaterial() - Failed to find material. Shader Type: " + std::string(magic_enum::enum_name(shaderType)));
		return Material();
	}

	const std::filesystem::path& Flux::AssetHandler::GetAudioPath(const std::string& audioName)
	{
		if (auto it = audioPaths.find(audioName); it != audioPaths.end()) { return it->second; }

		Debug::LogError("AssetHandler::GetAudioPath() - Failed to find audio path. Audio Name: " + audioName);
		return EMPTY_PATH;
	}

	bool AssetHandler::HasAudioPath(const std::string& audioName)
	{
		return audioPaths.contains(audioName);
	}

	std::unordered_map<std::string, ComPtr<ID3D11ShaderResourceView>>& AssetHandler::GetTextures(bool isSkyboxTextures)
	{
		if (isSkyboxTextures)
		{
			return skyboxTextures;
		}
		else
		{
			return textures;
		}
	}

	std::unordered_map<std::string, std::unique_ptr<Model>>& AssetHandler::GetModels()
	{
		return models;
	}

	const std::filesystem::path& AssetHandler::GetScenePath(const std::string& sceneName)
	{
		if (auto it = scenePaths.find(sceneName); it != scenePaths.end()) { return it->second; }

		Debug::LogError("AssetHandler::GetScenePath() - Failed to find scene path. Scene Name: " + sceneName);
		return EMPTY_PATH;
	}

	int AssetHandler::GetSceneCount()
	{
		return static_cast<int>(scenePaths.size());
	}

	HRESULT AssetHandler::LoadShaders(ShaderType shaderType, const std::filesystem::path& vertexShaderPath, const std::filesystem::path& pixelShaderPath)
	{
		HRESULT hResult = S_OK;

		if (FAILED(VerifyDeviceAndContext()))
		{
			Debug::LogError("AssetHandler::LoadShaders() - Unable to load shaders, see above error codes");
			return E_FAIL;
		}

		ID3D11Device& deviceRef = device->get();

		// INFO: Vertex Shader Loading
		std::vector<uint8_t> vertexShaderBytecode = DX::ReadData(vertexShaderPath.c_str());

		ComPtr<ID3D11VertexShader> vertexShader;
		hResult = deviceRef.CreateVertexShader(vertexShaderBytecode.data(), vertexShaderBytecode.size(), nullptr, &vertexShader);

		if (FAILED(hResult))
		{
			Debug::LogError("AssetHandler::LoadShaders() - Failed to create vertex shader. Filepath: " + vertexShaderPath.string());
			return hResult;
		}

		// INFO: Vertex Shader Reflection Setup
		ComPtr<ID3D11ShaderReflection> vertexShaderReflection;
		hResult = D3DReflect(vertexShaderBytecode.data(), vertexShaderBytecode.size(), IID_ID3D11ShaderReflection, &vertexShaderReflection);

		if (FAILED(hResult))
		{
			Debug::LogError("AssetHandler::LoadShaders() - Failed to reflect vertex shader. Filepath: " + vertexShaderPath.string());
			return hResult;
		}

		D3D11_SHADER_DESC vertexShaderDescription = {};
		hResult = vertexShaderReflection->GetDesc(&vertexShaderDescription);

		if (FAILED(hResult))
		{
			Debug::LogError("AssetHandler::LoadShaders() - Failed to get vertex shader description. Filepath: " + vertexShaderPath.string());
			return hResult;
		}

		auto signatureParameterDescriptions = std::make_unique<D3D11_SIGNATURE_PARAMETER_DESC[]>(vertexShaderDescription.InputParameters);

		for (UINT i = 0; i < vertexShaderDescription.InputParameters; ++i)
		{
			vertexShaderReflection->GetInputParameterDesc(i, &signatureParameterDescriptions[i]);
		}

		auto inputElementDescriptions = std::make_unique<D3D11_INPUT_ELEMENT_DESC[]>(vertexShaderDescription.InputParameters);

		for (size_t i = 0; i < vertexShaderDescription.InputParameters; ++i)
		{
			inputElementDescriptions[i].SemanticName = signatureParameterDescriptions[i].SemanticName;
			inputElementDescriptions[i].SemanticIndex = signatureParameterDescriptions[i].SemanticIndex;

			if (signatureParameterDescriptions[i].ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
			{
				switch (signatureParameterDescriptions[i].Mask)
				{
				case 1: inputElementDescriptions[i].Format = DXGI_FORMAT_R32_FLOAT; break;
				case 3: inputElementDescriptions[i].Format = DXGI_FORMAT_R32G32_FLOAT; break;
				case 7: inputElementDescriptions[i].Format = DXGI_FORMAT_R32G32B32_FLOAT; break;
				case 15: inputElementDescriptions[i].Format = DXGI_FORMAT_R32G32B32A32_FLOAT; break;
				default: break;
				}
			}

			inputElementDescriptions[i].InputSlot = 0;
			inputElementDescriptions[i].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			inputElementDescriptions[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			inputElementDescriptions[i].InstanceDataStepRate = 0;
		}

		ComPtr<ID3D11InputLayout> inputLayout;
		hResult = deviceRef.CreateInputLayout(inputElementDescriptions.get(), vertexShaderDescription.InputParameters, vertexShaderBytecode.data(), vertexShaderBytecode.size(), &inputLayout);

		if (FAILED(hResult))
		{
			Debug::LogError("AssetHandler::LoadShaders() - Failed to create input layout. Filepath: " + vertexShaderPath.string());
			return hResult;
		}

		// INFO: Pixel Shader Loading
		std::vector<uint8_t> pixelShaderBytecode = DX::ReadData(pixelShaderPath.c_str());

		ComPtr<ID3D11PixelShader> pixelShader;
		hResult = deviceRef.CreatePixelShader(pixelShaderBytecode.data(), pixelShaderBytecode.size(), nullptr, &pixelShader);

		if (FAILED(hResult))
		{
			Debug::LogError("AssetHandler::LoadShaders() - Failed to create pixel shader. Filepath: " + pixelShaderPath.string());
			return hResult;
		}

		// INFO: Store Shader Data (Vertex Shader, Pixel Shader, Input Layout)
		if (!shaders.try_emplace(shaderType, shaderType, vertexShader, pixelShader, inputLayout).second)
		{
			Debug::LogError("AssetHandler::LoadShaders() - Failed to insert shader data into map. Shader Type: " + std::string(magic_enum::enum_name(shaderType)));
			return E_FAIL;
		}

		return S_OK;
	}

	HRESULT AssetHandler::LoadConstantBuffer(ConstantBufferType constantBufferType)
	{
		HRESULT hResult = S_OK;

		if (FAILED(VerifyDeviceAndContext()))
		{
			Debug::LogError("AssetHandler::LoadConstantBuffer() - Unable to load constant buffer, see above error codes");
			return E_FAIL;
		}

		ID3D11Device& deviceRef = device->get();

		// INFO: Create Constant Buffer
		D3D11_BUFFER_DESC constantBufferDescription = { 0 };

		if (constantBufferType == ConstantBufferType::Unlit)
		{
			constantBufferDescription.ByteWidth = sizeof(ConstantBuffers::UnlitVS);
		}
		else
		{
			Debug::LogError("AssetHandler::LoadConstantBuffer() - Invalid constant buffer type. Constant Buffer Type: " + std::string(magic_enum::enum_name(constantBufferType)));
			return E_FAIL;
		}

		constantBufferDescription.Usage = D3D11_USAGE_DEFAULT;
		constantBufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constantBufferDescription.CPUAccessFlags = 0;
		constantBufferDescription.MiscFlags = 0;
		constantBufferDescription.StructureByteStride = 0;

		ComPtr<ID3D11Buffer> constantBuffer;
		hResult = deviceRef.CreateBuffer(&constantBufferDescription, nullptr, &constantBuffer);

		if (FAILED(hResult))
		{
			Debug::LogError("AssetHandler::LoadConstantBuffer() - Failed to create constant buffer. Constant Buffer Type: " + std::string(magic_enum::enum_name(constantBufferType)));
			return hResult;
		}

		if (ConstantBufferData constantBufferData(constantBufferType, constantBuffer); !constantBuffers.try_emplace(constantBufferType, constantBufferData).second)
		{
			Debug::LogError("AssetHandler::LoadConstantBuffer() - Failed to insert constant buffer data into map. Constant Buffer Type: " + std::string(magic_enum::enum_name(constantBufferType)));
			return E_FAIL;
		}

		return S_OK;
	}

	HRESULT AssetHandler::LoadDepthWriteState(DepthWriteType depthWriteType)
	{
		HRESULT hResult = S_OK;

		if (FAILED(VerifyDeviceAndContext()))
		{
			Debug::LogError("AssetHandler::LoadDepthWriteState() - Unable to load depth write state, see above error codes");
			return E_FAIL;
		}

		ID3D11Device& deviceRef = device->get();

		// INFO: Create Depth Write State
		D3D11_DEPTH_STENCIL_DESC depthStencilDescription = { 0 };
		depthStencilDescription.DepthEnable = TRUE;

		switch (depthWriteType)
		{
		using enum Flux::DirectXConfig::DepthWriteType;
		case Enabled:
			depthStencilDescription.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			break;
		case Disabled:
			depthStencilDescription.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
			break;
		case None:
		default:
			break;
		}

		depthStencilDescription.DepthFunc = D3D11_COMPARISON_LESS;
		depthStencilDescription.StencilEnable = FALSE;
		depthStencilDescription.StencilReadMask = 0;
		depthStencilDescription.StencilWriteMask = 0;

		depthStencilDescription.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDescription.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDescription.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDescription.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		depthStencilDescription.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDescription.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDescription.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDescription.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		ComPtr<ID3D11DepthStencilState> depthStencilState;
		hResult = deviceRef.CreateDepthStencilState(&depthStencilDescription, &depthStencilState);

		if (FAILED(hResult))
		{
			Debug::LogError("AssetHandler::LoadDepthWriteState() - Failed to create depth write state. Depth Write Type: " + std::string(magic_enum::enum_name(depthWriteType)));
			return hResult;
		}

		if (!depthWriteStates.insert({ depthWriteType, std::move(depthStencilState) }).second)
		{
			Debug::LogError("AssetHandler::LoadDepthWriteState() - Failed to insert depth write state into map. Depth Write Type: " + std::string(magic_enum::enum_name(depthWriteType)));
			return E_FAIL;
		}

		return S_OK;
	}

	HRESULT AssetHandler::LoadCullingModeState(CullingModeType cullingModeType)
	{
		HRESULT hResult = S_OK;

		if (FAILED(VerifyDeviceAndContext()))
		{
			Debug::LogError("AssetHandler::LoadCullingModeState() - Unable to load culling mode state, see above error codes");
			return E_FAIL;
		}

		ID3D11Device& deviceRef = device->get();

		// INFO: Create Culling Mode State
		D3D11_RASTERIZER_DESC rasterizerDescription = {};
		ZeroMemory(&rasterizerDescription, sizeof(D3D11_RASTERIZER_DESC));

		switch (cullingModeType)
		{
		using enum Flux::DirectXConfig::CullingModeType;
		case FrontSolid:
			rasterizerDescription.FillMode = D3D11_FILL_SOLID;
			rasterizerDescription.CullMode = D3D11_CULL_FRONT;
			break;
		case BackSolid:
			rasterizerDescription.FillMode = D3D11_FILL_SOLID;
			rasterizerDescription.CullMode = D3D11_CULL_BACK;
			break;
		case None:
		default:
			break;
		}

		rasterizerDescription.FrontCounterClockwise = FALSE;
		rasterizerDescription.DepthBias = 0;
		rasterizerDescription.DepthBiasClamp = 0.0f;
		rasterizerDescription.SlopeScaledDepthBias = 0.0f;
		rasterizerDescription.DepthClipEnable = TRUE;
		rasterizerDescription.ScissorEnable = FALSE;
		rasterizerDescription.MultisampleEnable = FALSE;
		rasterizerDescription.AntialiasedLineEnable = FALSE;

		ComPtr<ID3D11RasterizerState> rasterizerState;
		hResult = deviceRef.CreateRasterizerState(&rasterizerDescription, &rasterizerState);

		if (FAILED(hResult))
		{
			Debug::LogError("AssetHandler::LoadCullingModeState() - Failed to create culling mode state. Culling Mode Type: " + std::string(magic_enum::enum_name(cullingModeType)));
			return hResult;
		}

		if (!cullingModeStates.insert({ cullingModeType, std::move(rasterizerState) }).second)
		{
			Debug::LogError("AssetHandler::LoadCullingModeState() - Failed to insert culling mode state into map. Culling Mode Type: " + std::string(magic_enum::enum_name(cullingModeType)));
			return E_FAIL;
		}

		return S_OK;
	}

	HRESULT AssetHandler::LoadSamplerState()
	{
		HRESULT hResult = S_OK;

		if (FAILED(VerifyDeviceAndContext()))
		{
			Debug::LogError("AssetHandler::LoadSamplerState() - Unable to load sampler state, see above error codes");
			return E_FAIL;
		}

		ID3D11Device& deviceRef = device->get();

		// INFO: Create Sampler State
		D3D11_SAMPLER_DESC samplerDescription = {};
		ZeroMemory(&samplerDescription, sizeof(D3D11_SAMPLER_DESC));

		samplerDescription.Filter = D3D11_FILTER_ANISOTROPIC;
		samplerDescription.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDescription.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDescription.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDescription.MipLODBias = 0.0f;
		samplerDescription.MaxAnisotropy = D3D11_MAX_MAXANISOTROPY;
		samplerDescription.ComparisonFunc = D3D11_COMPARISON_NEVER;

		samplerDescription.BorderColor[0] = 0;
		samplerDescription.BorderColor[1] = 0;
		samplerDescription.BorderColor[2] = 0;
		samplerDescription.BorderColor[3] = 0;

		samplerDescription.MinLOD = 0;
		samplerDescription.MaxLOD = D3D11_FLOAT32_MAX;

		hResult = deviceRef.CreateSamplerState(&samplerDescription, &samplerState);
		if (FAILED(hResult))
		{
			Debug::LogError("AssetHandler::LoadSamplerState() - Failed to create sampler state");
			return hResult;
		}

		return S_OK;
	}

	int AssetHandler::LoadMaterial(ShaderType shaderType)
	{
		// INFO: Create Material
		Material material;

		switch (shaderType)
		{
		case ShaderType::Unlit:
			material = Material(ShaderType::Unlit, ConstantBufferType::Unlit, DepthWriteType::Enabled, CullingModeType::BackSolid, "DefaultTexture");
			break;
		case ShaderType::Skybox:
			material = Material(ShaderType::Skybox, ConstantBufferType::Unlit, DepthWriteType::Disabled, CullingModeType::FrontSolid, "DefaultSkybox", true);
			break;
		case ShaderType::None:
		default:
			break;
		}

		if (!materials.try_emplace(shaderType, material).second)
		{
			Debug::LogError("AssetHandler::LoadMaterial() - Failed to insert material into map. Shader Type: " + std::string(magic_enum::enum_name(shaderType)));
			return FLUX_FAILURE;
		}

		return FLUX_SUCCESS;
	}

	HRESULT AssetHandler::VerifyDeviceAndContext(bool verifyContext)
	{
		HRESULT hResult = S_OK;

		if (!device.has_value())
		{
			Debug::LogError("AssetHandler::VerifyDeviceAndContext() - Device is not initialised");
			hResult = E_FAIL;
		}

		if (verifyContext && !deviceContext.has_value())
		{
			Debug::LogError("AssetHandler::VerifyDeviceAndContext() - Device Context is not initialised");
			hResult = E_FAIL;
		}

		return hResult;
	}
}
