#pragma once

#include <d3d11.h>
#include <memory>
#include <wrl.h>

#include "Core/Configs/DirectXConfig.h"

namespace Flux
{
	/// @brief Class to hold the shader data in the asset handler
	class ShaderData
	{
	public:
		ShaderData() : shaderType(DirectXConfig::ShaderType::None), vertexShader(nullptr), pixelShader(nullptr), inputLayout(nullptr) {}
		ShaderData(DirectXConfig::ShaderType _shaderType, Microsoft::WRL::ComPtr<ID3D11VertexShader> _vertexShader, Microsoft::WRL::ComPtr<ID3D11PixelShader> _pixelShader, 
				   Microsoft::WRL::ComPtr<ID3D11InputLayout> _inputLayout) : shaderType(_shaderType), vertexShader(std::move(_vertexShader)), 
																			 pixelShader(std::move(_pixelShader)), inputLayout(std::move(_inputLayout)) {}
		~ShaderData() = default;

		inline DirectXConfig::ShaderType GetShaderType() const { return shaderType; }
		inline ID3D11VertexShader* GetVertexShader() { return vertexShader.Get(); }
		inline ID3D11PixelShader* GetPixelShader() { return pixelShader.Get(); }
		inline ID3D11InputLayout* GetInputLayout() { return inputLayout.Get(); }

	public:
		static const ShaderData EMPTY;

	private:
		DirectXConfig::ShaderType shaderType;
		Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	};

	inline const ShaderData ShaderData::EMPTY = {};
}

