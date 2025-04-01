#pragma once

#include <d3d11.h>
#include <memory>
#include <wrl.h>

#include "Core/Configs/DirectXConfig.h"

namespace Flux
{
	/// @brief Class to hold the constant buffer data in the asset handler
	struct ConstantBufferData
	{
	public:
		ConstantBufferData() : constantBufferType(DirectXConfig::ConstantBufferType::None), buffer(nullptr) {}
		ConstantBufferData(DirectXConfig::ConstantBufferType _constantBufferType, Microsoft::WRL::ComPtr<ID3D11Buffer> _buffer) : constantBufferType(_constantBufferType), 
																																  buffer(std::move(_buffer)) {}
		~ConstantBufferData() = default;

		inline DirectXConfig::ConstantBufferType GetConstantBufferType() const { return constantBufferType; }
		inline ID3D11Buffer* GetConstantBuffer() { return buffer.Get(); }

	public:
		static ConstantBufferData EMPTY;

	private:
		DirectXConfig::ConstantBufferType constantBufferType;
		Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
	};

	inline ConstantBufferData ConstantBufferData::EMPTY{};
}

