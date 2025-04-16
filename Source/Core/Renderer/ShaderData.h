#pragma once

#include <wrl.h>

#include "Core/Configs/DirectXConfig.h"

struct ID3D11InputLayout;
struct ID3D11PixelShader;
struct ID3D11VertexShader;

namespace Flux
{
	/// @brief Class to hold the shader data in the asset handler
	class ShaderData
	{
	public:
		ShaderData();
		ShaderData(DirectXConfig::ShaderType _shaderType, Microsoft::WRL::ComPtr<ID3D11VertexShader> _vertexShader, Microsoft::WRL::ComPtr<ID3D11PixelShader> _pixelShader,
				   Microsoft::WRL::ComPtr<ID3D11InputLayout> _inputLayout);
		~ShaderData();

		DirectXConfig::ShaderType GetShaderType() const;
		ID3D11VertexShader* GetVertexShader();
		ID3D11PixelShader* GetPixelShader();
		ID3D11InputLayout* GetInputLayout();

	private:
		DirectXConfig::ShaderType shaderType;
		Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	};
}

