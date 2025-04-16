#pragma once

#include <string>

#include "Core/Configs/DirectXConfig.h"

struct ID3D11Buffer;
struct ID3D11DepthStencilState;
struct ID3D11DeviceContext;
struct ID3D11InputLayout;
struct ID3D11PixelShader;
struct ID3D11RasterizerState;
struct ID3D11SamplerState;
struct ID3D11ShaderResourceView;
struct ID3D11VertexShader;

namespace Flux
{
	class Material
	{
	public:
		Material() = default;
		Material(DirectXConfig::ShaderType shaderType, DirectXConfig::ConstantBufferType _constantBufferType, DirectXConfig::DepthWriteType depthWriteType,
			     DirectXConfig::CullingModeType cullingModeType, const std::string& textureName, bool isSkyboxTexture = false);
		~Material();

		DirectXConfig::ConstantBufferType GetConstantBufferType() const;
		ID3D11Buffer* GetConstantBuffer() const;

		void SetTexture(const std::string& textureName, bool isSkyboxTexture = false);

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

