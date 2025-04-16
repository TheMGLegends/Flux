#include "Material.h"

#include <d3d11.h>

#include "Core/Debug/Debug.h"
#include "Core/Renderer/AssetHandler.h"
#include "Core/Renderer/ConstantBufferData.h"
#include "Core/Renderer/ShaderData.h"

namespace Flux
{
	using namespace DirectXConfig;

	Material::Material(ShaderType shaderType, ConstantBufferType _constantBufferType, DepthWriteType depthWriteType,
					   CullingModeType cullingModeType, const std::string& textureName, bool isSkyboxTexture)
	{
		ShaderData& shaderData = AssetHandler::GetShaderData(shaderType);
		vertexShader = shaderData.GetVertexShader();
		pixelShader = shaderData.GetPixelShader();
		inputLayout = shaderData.GetInputLayout();

		ConstantBufferData& constantBufferData = AssetHandler::GetConstantBufferData(_constantBufferType);
		constantBuffer = constantBufferData.GetConstantBuffer();
		constantBufferType = constantBufferData.GetConstantBufferType();

		depthWrite = AssetHandler::GetDepthWriteState(depthWriteType);
		cullingMode = AssetHandler::GetCullingModeState(cullingModeType);

		texture = AssetHandler::GetTexture(textureName, isSkyboxTexture);
		sampler = AssetHandler::GetSamplerState();
	}

	Material::~Material()
	{
	}

	ConstantBufferType Material::GetConstantBufferType() const
	{
		return constantBufferType;
	}

	ID3D11Buffer* Material::GetConstantBuffer() const
	{
		return constantBuffer;
	}

	void Material::SetTexture(const std::string& textureName, bool isSkyboxTexture)
	{
		texture = AssetHandler::GetTexture(textureName, isSkyboxTexture);

		if (!texture) { Debug::LogError("Material::SetTexture() - Failed to load Texture: " + textureName); }
	}

	void Material::Bind(ID3D11DeviceContext& deviceContext)
	{
		if (inputLayout)	{ deviceContext.IASetInputLayout(inputLayout); }
		if (vertexShader)	{ deviceContext.VSSetShader(vertexShader, nullptr, 0); }
		if (pixelShader)	{ deviceContext.PSSetShader(pixelShader, nullptr, 0); }
		if (constantBuffer) { deviceContext.VSSetConstantBuffers(0, 1, &constantBuffer); }
		if (depthWrite)		{ deviceContext.OMSetDepthStencilState(depthWrite, 0); }
		if (cullingMode)	{ deviceContext.RSSetState(cullingMode); }
		if (texture)		{ deviceContext.PSSetShaderResources(0, 1, &texture); }
		if (sampler)		{ deviceContext.PSSetSamplers(0, 1, &sampler); }
	}
}
