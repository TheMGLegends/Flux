#include "Material.h"

#include "../Debug/Debug.h"

using namespace DirectXConfig;

Material::Material(ShaderType shaderType, ConstantBufferType constantBufferType, DepthWriteType depthWriteType, 
	               CullingModeType cullingModeType, const std::string& textureName) : vertexShader(nullptr), pixelShader(nullptr), 
																					  inputLayout(nullptr), depthWrite(nullptr), 
																				      cullingMode(nullptr), texture(nullptr), sampler(nullptr)
{
	// TODO: Asset Handler to load all things
}

Material::~Material()
{
}

void Material::Bind(ID3D11DeviceContext* deviceContext)
{
	if (!deviceContext)
	{
		Debug::LogError("Material::Bind() - Invalid device context");
		return;
	}

	if (inputLayout)
		deviceContext->IASetInputLayout(inputLayout);
	
	if (vertexShader)
		deviceContext->VSSetShader(vertexShader, nullptr, 0);

	if (pixelShader)
		deviceContext->PSSetShader(pixelShader, nullptr, 0);

	if (constantBufferData.buffer)
		deviceContext->VSSetConstantBuffers(0, 1, &constantBufferData.buffer);

	if (depthWrite)
		deviceContext->OMSetDepthStencilState(depthWrite, 0);

	if (cullingMode)
		deviceContext->RSSetState(cullingMode);

	if (texture)
		deviceContext->PSSetShaderResources(0, 1, &texture);

	if (sampler)
		deviceContext->PSSetSamplers(0, 1, &sampler);
}
