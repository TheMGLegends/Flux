#pragma once

#include <d3d11.h>
#include <PrimitiveBatch.h>
#include <VertexTypes.h>

namespace Flux
{
	class IDebugWireframe
	{
	public:
		virtual ~IDebugWireframe() = default;

		virtual void DrawWireframe(ID3D11DeviceContext& deviceContext, DirectX::PrimitiveBatch<DirectX::VertexPositionColor>& primitiveBatch) = 0;
	};
}

