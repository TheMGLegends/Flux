#pragma once

#include <d3d11.h>
#include <string>

#include "Core/Configs/DirectXConfig.h"

namespace Flux
{
	class Material
	{
	public:
		Material() = default;
		Material(DirectXConfig::ShaderType shaderType, DirectXConfig::ConstantBufferType _constantBufferType, DirectXConfig::DepthWriteType depthWriteType,
			     DirectXConfig::CullingModeType cullingModeType, const std::string& textureName);
		~Material();

		inline DirectXConfig::ConstantBufferType GetConstantBufferType() const { return constantBufferType; }
		inline ID3D11Buffer* GetConstantBuffer() const { return constantBuffer; }

		void SetTexture(const std::string& textureName);

		void Bind(ID3D11DeviceContext& deviceContext);

	private:
		ID3D11VertexShader* vertexShader; // INFO: Populated by AssetHandler from ShaderData
		ID3D11PixelShader* pixelShader; // INFO: Populated by AssetHandler from ShaderData
		ID3D11InputLayout* inputLayout; // INFO: Populated by AssetHandler from ShaderData

		DirectXConfig::ConstantBufferType constantBufferType; // INFO: Populated by AssetHandler from ConstantBufferData
		ID3D11Buffer* constantBuffer; // INFO: Populated by AssetHandler from ConstantBufferData

		ID3D11DepthStencilState* depthWrite;
		ID3D11RasterizerState* cullingMode;

		ID3D11ShaderResourceView* texture;
		ID3D11SamplerState* sampler;
	};
}

