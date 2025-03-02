#pragma once

#include "../Globals.h"

#include <d3d11.h>
#include <string>

struct ConstantBufferData
{
public:
	ConstantBufferData() : constantBufferType(DirectXConfig::ConstantBufferType::None), buffer(nullptr) {}
	ConstantBufferData(DirectXConfig::ConstantBufferType _constantBufferType, ID3D11Buffer* _buffer) : constantBufferType(_constantBufferType), 
																											   buffer(_buffer) {}

public:
	DirectXConfig::ConstantBufferType constantBufferType;
	ID3D11Buffer* buffer;
};

class Material
{
public:
	Material(DirectXConfig::ShaderType shaderType, DirectXConfig::ConstantBufferType constantBufferType, DirectXConfig::DepthWriteType depthWriteType,
		     DirectXConfig::CullingModeType cullingModeType, const std::string& textureName);
	~Material();

	inline ConstantBufferData& GetConstantBufferData() { return constantBufferData; }

	void Bind(ID3D11DeviceContext* deviceContext);

private:
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11InputLayout* inputLayout;

	ConstantBufferData constantBufferData;

	ID3D11DepthStencilState* depthWrite;
	ID3D11RasterizerState* cullingMode;

	ID3D11ShaderResourceView* texture;
	ID3D11SamplerState* sampler;
};

