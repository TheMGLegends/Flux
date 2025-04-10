#pragma once

#include <wrl.h>

#include "Core/Configs/DirectXConfig.h"

struct ID3D11Buffer;

namespace Flux
{
	/// @brief Class to hold the constant buffer data in the asset handler
	struct ConstantBufferData
	{
	public:
		ConstantBufferData();
		ConstantBufferData(DirectXConfig::ConstantBufferType _constantBufferType, Microsoft::WRL::ComPtr<ID3D11Buffer> _buffer);
		~ConstantBufferData();

		DirectXConfig::ConstantBufferType GetConstantBufferType() const { return constantBufferType; }
		ID3D11Buffer* GetConstantBuffer() { return buffer.Get(); }

	private:
		DirectXConfig::ConstantBufferType constantBufferType;
		Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
	};
}

