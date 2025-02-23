#pragma once

#include <d3d11.h>

class IDebugWireframe
{
public:
	virtual ~IDebugWireframe() = default;

	virtual void DrawWireframe(ID3D11DeviceContext* deviceContext) = 0;
};

