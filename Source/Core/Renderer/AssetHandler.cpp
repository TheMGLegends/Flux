#include "AssetHandler.h"

#pragma warning (push, 0)
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#pragma warning (pop)
#include <DDSTextureLoader.h>
#include <d3dcompiler.h>
#include <d3d11shader.h>
#include <fmod.hpp>
#include <fmod_errors.h>
#include <magic_enum.hpp>
#include <WICTextureLoader.h>

#include "ConstantBuffers.h"
#include "Material.h"
#include "Model.h"
#include "ReadData.h"
#include "Core/GlobalDefines.h"
#include "Core/Configs/FiletypeConfig.h"
#include "Core/Debug/Debug.h"
#include "Core/Helpers/StringHelpers.h"

constexpr const char* const ASSET_DIRECTORY = "Assets";

using namespace Flux;
using namespace Flux::DirectXConfig;
using namespace Flux::GlobalDefines;
using namespace Microsoft::WRL;

const std::filesystem::path AssetHandler::EMPTY_PATH = std::filesystem::path();

std::optional<std::reference_wrapper<ID3D11Device>> AssetHandler::device;
std::optional<std::reference_wrapper<ID3D11DeviceContext>> AssetHandler::deviceContext;

std::unordered_map<ShaderType, ShaderData> AssetHandler::shaders;
std::unordered_map<ConstantBufferType, ConstantBufferData> AssetHandler::constantBuffers;
std::unordered_map<DepthWriteType, ComPtr<ID3D11DepthStencilState>> AssetHandler::depthWriteStates;
std::unordered_map<CullingModeType, ComPtr<ID3D11RasterizerState>> AssetHandler::cullingModeStates;

ComPtr<ID3D11SamplerState> AssetHandler::samplerState;

std::unordered_map<std::string, std::unique_ptr<DirectX::SpriteFont>> AssetHandler::fonts;
std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> AssetHandler::textures;
std::unordered_map<std::string, std::unique_ptr<Model>> AssetHandler::models;
std::unordered_map<DirectXConfig::ShaderType, Material> AssetHandler::materials;
std::unordered_map<std::string, std::filesystem::path> AssetHandler::audioPaths;

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

	LoadAssets(ASSET_DIRECTORY);

	if (IS_FAILURE(LoadMaterial(ShaderType::Unlit))) { return E_FAIL; }
	if (IS_FAILURE(LoadMaterial(ShaderType::Skybox))) { return E_FAIL; }

	return S_OK;
}

HRESULT AssetHandler::LoadAssets(const std::filesystem::path& assetDirectory)
{
	Assimp::Importer importer;

	// INFO: Go through all files and subdirectories in the specified directory and load assets
	for (const auto& entry : std::filesystem::recursive_directory_iterator(assetDirectory))
	{
		// INFO: Don't bother with checking directories (Folders)
		if (entry.is_regular_file())
		{
			std::string extensionType = StringHelpers::ToLower(entry.path().extension().string());

			// INFO: Font Loading
			if (extensionType == FiletypeConfig::SPRITEFONT)
			{
				if (IS_FAILURE(LoadFont(entry.path()))) { return E_FAIL; }
				continue;
			}

			// INFO: Texture Loading
			if (FiletypeConfig::IsSupportedTextureFormat(extensionType))
			{
				if (FAILED(LoadTexture(entry.path()))) { return E_FAIL; }
				continue;
			}

			// INFO: Model Loading
			if (FiletypeConfig::IsSupportedModelFormat(extensionType))
			{
				if (IS_FAILURE(LoadModel(entry.path(), importer))) { return E_FAIL; }
				continue;
			}

			// INFO: Audio Loading
			if (FiletypeConfig::IsSupportedAudioFormat(extensionType))
			{
				if (IS_FAILURE(LoadAudio(entry.path()))) { return E_FAIL; }
				continue;
			}
		}
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

	auto& deviceRef = device->get();

	std::unique_ptr<DirectX::SpriteFont> font = std::make_unique<DirectX::SpriteFont>(&deviceRef, fontPath.c_str());

	if (!font)
	{
		Debug::LogError("AssetHandler::LoadFont() - Failed to create font. Filepath: " + fontPath.string());
		return FLUX_FAILURE;
	}

	if (!fonts.insert({ fontPath.stem().string(), std::move(font) }).second)
	{
		Debug::LogError("AssetHandler::LoadFont() - Failed to insert font into map. Filepath: " + fontPath.string());
		return FLUX_FAILURE;
	}

	return FLUX_SUCCESS;
}

HRESULT AssetHandler::LoadTexture(const std::filesystem::path& texturePath)
{
	HRESULT hResult = S_OK;

	hResult = VerifyDeviceAndContext(true);

	if (FAILED(hResult))
	{
		Debug::LogError("AssetHandler::LoadTexture() - Unable to load texture, see above error codes");
		return hResult;
	}

	auto& deviceRef = device->get();
	auto& deviceContextRef = deviceContext->get();

	ComPtr<ID3D11ShaderResourceView> texture;

	std::string extensionType = StringHelpers::ToLower(texturePath.extension().string());

	// INFO: DDS Mainly used for Skybox Textures
	if (extensionType == FiletypeConfig::DDS)
	{
		hResult = DirectX::CreateDDSTextureFromFile(&deviceRef, &deviceContextRef, texturePath.c_str(), nullptr, &texture);
	}
	else
	{
		hResult = DirectX::CreateWICTextureFromFile(&deviceRef, &deviceContextRef, texturePath.c_str(), nullptr, &texture);
	}

	if (FAILED(hResult))
	{
		Debug::LogError("AssetHandler::LoadTexture() - Failed to create texture. Filepath: " + texturePath.string());
		return hResult;
	}

	if (!textures.insert({ texturePath.stem().string(), std::move(texture) }).second)
	{
		Debug::LogError("AssetHandler::LoadTexture() - Failed to insert texture into map. Filepath: " + texturePath.string());
		return E_FAIL;
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

	auto& deviceRef = device->get();
	auto& deviceContextRef = deviceContext->get();

	const aiScene* scene = importer.ReadFile(modelPath.string(), aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_ConvertToLeftHanded);

	if (!scene)
	{
		Debug::LogError("AssetHandler::LoadModel() - Failed to load model. Filepath: " + modelPath.string());
		return FLUX_FAILURE;
	}

	std::unique_ptr<Model> model = std::make_unique<Model>(deviceRef, deviceContextRef, scene, modelPath.stem().string());

	if (!model)
	{
		Debug::LogError("AssetHandler::LoadModel() - Failed to create model. Filepath: " + modelPath.string());
		return FLUX_FAILURE;
	}

	if (!models.insert({ modelPath.stem().string(), std::move(model) }).second)
	{
		Debug::LogError("AssetHandler::LoadModel() - Failed to insert model into map. Filepath: " + modelPath.string());
		return FLUX_FAILURE;
	}

	return FLUX_SUCCESS;
}

int AssetHandler::LoadAudio(const std::filesystem::path& audioPath)
{
	if (!audioPaths.insert({ audioPath.stem().string(), audioPath }).second)
	{
		Debug::LogError("AssetHandler::LoadAudio() - Failed to insert audio path into map. Filepath: " + audioPath.string());
		return FLUX_FAILURE;
	}

	return FLUX_SUCCESS;
}

ShaderData& AssetHandler::GetShaderData(DirectXConfig::ShaderType shaderType)
{
	auto it = shaders.find(shaderType);
	if (it != shaders.end()) { return it->second; }

	Debug::LogError("AssetHandler::GetShaderData() - Failed to find shader data. Shader Type: " + std::to_string(magic_enum::enum_integer(shaderType)));
	return ShaderData::EMPTY;
}

ConstantBufferData& AssetHandler::GetConstantBufferData(DirectXConfig::ConstantBufferType constantBufferType)
{
	auto it = constantBuffers.find(constantBufferType);
	if (it != constantBuffers.end()) { return it->second; }

	Debug::LogError("AssetHandler::GetConstantBufferData() - Failed to find constant buffer data. Constant Buffer Type: " + std::to_string(magic_enum::enum_integer(constantBufferType)));
	return ConstantBufferData::EMPTY;
}

ID3D11DepthStencilState* AssetHandler::GetDepthWriteState(DirectXConfig::DepthWriteType depthWriteType)
{
	auto it = depthWriteStates.find(depthWriteType);
	if (it != depthWriteStates.end()) { return it->second.Get(); }

	Debug::LogError("AssetHandler::GetDepthWriteState() - Failed to find depth write state. Depth Write Type: " + std::to_string(magic_enum::enum_integer(depthWriteType)));
	return nullptr;
}

ID3D11RasterizerState* AssetHandler::GetCullingModeState(DirectXConfig::CullingModeType cullingModeType)
{
	auto it = cullingModeStates.find(cullingModeType);
	if (it != cullingModeStates.end()) { return it->second.Get(); }

	Debug::LogError("AssetHandler::GetCullingModeState() - Failed to find culling mode state. Culling Mode Type: " + std::to_string(magic_enum::enum_integer(cullingModeType)));
	return nullptr;
}

DirectX::SpriteFont* AssetHandler::GetFont(const std::string& fontName)
{
	auto it = fonts.find(fontName);
	if (it != fonts.end()) { return it->second.get(); }

	Debug::LogError("AssetHandler::GetFont() - Failed to find font. Font Name: " + fontName);
	return nullptr;
}

ID3D11ShaderResourceView* AssetHandler::GetTexture(const std::string& textureName)
{
	auto it = textures.find(textureName);
	if (it != textures.end()) { return it->second.Get(); }

	Debug::LogError("AssetHandler::GetTexture() - Failed to find texture. Texture Name: " + textureName);
	return nullptr;
}

Model* AssetHandler::GetModel(const std::string& modelName)
{
	auto it = models.find(modelName);
	if (it != models.end()) { return it->second.get(); }

	Debug::LogError("AssetHandler::GetModel() - Failed to find model. Model Name: " + modelName);
	return nullptr;
}

Material AssetHandler::GetMaterial(DirectXConfig::ShaderType shaderType)
{
	auto it = materials.find(shaderType);
	if (it != materials.end()) { return it->second; }

	Debug::LogError("AssetHandler::GetMaterial() - Failed to find material. Shader Type: " + std::string(magic_enum::enum_name(shaderType)));
	return Material();
}

const std::filesystem::path& Flux::AssetHandler::GetAudioPath(const std::string& audioName)
{
	auto it = audioPaths.find(audioName);
	if (it != audioPaths.end()) { return it->second; }

	Debug::LogError("AssetHandler::GetAudioPath() - Failed to find audio path. Audio Name: " + audioName);
	return EMPTY_PATH;
}

HRESULT AssetHandler::LoadShaders(ShaderType shaderType, const std::filesystem::path& vertexShaderPath, const std::filesystem::path& pixelShaderPath)
{
	HRESULT hResult = S_OK;

	if (FAILED(VerifyDeviceAndContext()))
	{
		Debug::LogError("AssetHandler::LoadShaders() - Unable to load shaders, see above error codes");
		return E_FAIL;
	}

	auto& deviceRef = device->get();

	// INFO: Vertex Shader Loading
	auto vertexShaderBytecode = DX::ReadData(vertexShaderPath.c_str());

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

	std::unique_ptr<D3D11_SIGNATURE_PARAMETER_DESC[]> signatureParameterDescriptions = std::make_unique<D3D11_SIGNATURE_PARAMETER_DESC[]>(vertexShaderDescription.InputParameters);
	
	for (UINT i = 0; i < vertexShaderDescription.InputParameters; ++i)
	{
		vertexShaderReflection->GetInputParameterDesc(i, &signatureParameterDescriptions[i]);
	}

	std::unique_ptr<D3D11_INPUT_ELEMENT_DESC[]> inputElementDescriptions = std::make_unique<D3D11_INPUT_ELEMENT_DESC[]>(vertexShaderDescription.InputParameters);

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
	auto pixelShaderBytecode = DX::ReadData(pixelShaderPath.c_str());

	ComPtr<ID3D11PixelShader> pixelShader;
	hResult = deviceRef.CreatePixelShader(pixelShaderBytecode.data(), pixelShaderBytecode.size(), nullptr, &pixelShader);

	if (FAILED(hResult))
	{
		Debug::LogError("AssetHandler::LoadShaders() - Failed to create pixel shader. Filepath: " + pixelShaderPath.string());
		return hResult;
	}

	// INFO: Store Shader Data (Vertex Shader, Pixel Shader, Input Layout)
	ShaderData shaderData(shaderType, vertexShader, pixelShader, inputLayout);

	if (!shaders.insert({ shaderType, shaderData }).second)
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

	auto& deviceRef = device->get();

	// INFO: Create Constant Buffer
	D3D11_BUFFER_DESC constantBufferDescription = { 0 };

	switch (constantBufferType)
	{
	case ConstantBufferType::Unlit:
		constantBufferDescription.ByteWidth = sizeof(ConstantBuffers::UnlitVS);
		break;
	case ConstantBufferType::None:
	default:
		break;
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

	ConstantBufferData constantBufferData(constantBufferType, constantBuffer);

	if (!constantBuffers.insert({ constantBufferType, constantBufferData }).second)
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

	auto& deviceRef = device->get();

	// INFO: Create Depth Write State
	D3D11_DEPTH_STENCIL_DESC depthStencilDescription = { 0 };
	depthStencilDescription.DepthEnable = TRUE;

	switch (depthWriteType)
	{
	case DepthWriteType::Enabled:
		depthStencilDescription.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		break;
	case DepthWriteType::Disabled:
		depthStencilDescription.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		break;
	case DepthWriteType::None:
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

	auto& deviceRef = device->get();

	// INFO: Create Culling Mode State
	D3D11_RASTERIZER_DESC rasterizerDescription = {};
	ZeroMemory(&rasterizerDescription, sizeof(D3D11_RASTERIZER_DESC));

	switch (cullingModeType)
	{
	case CullingModeType::FrontSolid:
		rasterizerDescription.FillMode = D3D11_FILL_SOLID;
		rasterizerDescription.CullMode = D3D11_CULL_FRONT;
		break;
	case CullingModeType::BackSolid:
		rasterizerDescription.FillMode = D3D11_FILL_SOLID;
		rasterizerDescription.CullMode = D3D11_CULL_BACK;
		break;
	case CullingModeType::None:
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

	auto& deviceRef = device->get();

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
		material = Material(ShaderType::Skybox, ConstantBufferType::Unlit, DepthWriteType::Disabled, CullingModeType::FrontSolid, "DefaultSkybox");
		break;
	case ShaderType::None:
	default:
		break;
	}

	if (!materials.insert({ shaderType, material }).second)
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

	if (verifyContext)
	{
		if (!deviceContext.has_value())
		{
			Debug::LogError("AssetHandler::VerifyDeviceAndContext() - Device Context is not initialised");
			hResult = E_FAIL;
		}
	}

	return hResult;
}
