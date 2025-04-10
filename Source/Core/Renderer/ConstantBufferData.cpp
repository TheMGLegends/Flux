#include "ConstantBufferData.h"

#include <d3d11.h>
#include <memory>

using namespace Microsoft::WRL;

namespace Flux
{
	ConstantBufferData::ConstantBufferData() : constantBufferType(DirectXConfig::ConstantBufferType::None), buffer(nullptr)
	{
	}

	ConstantBufferData::ConstantBufferData(DirectXConfig::ConstantBufferType _constantBufferType, ComPtr<ID3D11Buffer> _buffer)
		: constantBufferType(_constantBufferType), buffer(std::move(_buffer))
	{
	}

	ConstantBufferData::~ConstantBufferData()
	{
	}
}
