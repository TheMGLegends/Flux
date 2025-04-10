#include "ShaderData.h"

#include <d3d11.h>
#include <memory>

using namespace Microsoft::WRL;

namespace Flux
{
	ShaderData::ShaderData() : shaderType(DirectXConfig::ShaderType::None), vertexShader(nullptr), pixelShader(nullptr), inputLayout(nullptr)
	{
	}

	ShaderData::ShaderData(DirectXConfig::ShaderType _shaderType, ComPtr<ID3D11VertexShader> _vertexShader, 
						   ComPtr<ID3D11PixelShader> _pixelShader, ComPtr<ID3D11InputLayout> _inputLayout)
		: shaderType(_shaderType), vertexShader(std::move(_vertexShader)), pixelShader(std::move(_pixelShader)), inputLayout(std::move(_inputLayout))
	{
	}

	ShaderData::~ShaderData()
	{
	}
}
