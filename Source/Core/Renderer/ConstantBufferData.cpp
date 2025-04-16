#include "ConstantBufferData.h"

#include <d3d11.h>
#include <memory>

using namespace Microsoft::WRL;

namespace Flux
{
	using namespace DirectXConfig;

	ConstantBufferData::ConstantBufferData() : constantBufferType(ConstantBufferType::None), buffer(nullptr)
	{
	}

	ConstantBufferData::ConstantBufferData(ConstantBufferType _constantBufferType, ComPtr<ID3D11Buffer> _buffer)
		: constantBufferType(_constantBufferType), buffer(std::move(_buffer))
	{
	}

	ConstantBufferData::~ConstantBufferData()
	{
	}

	ConstantBufferType ConstantBufferData::GetConstantBufferType() const
	{
		return constantBufferType;
	}

	ID3D11Buffer* ConstantBufferData::GetConstantBuffer()
	{
		return buffer.Get();
	}
}
