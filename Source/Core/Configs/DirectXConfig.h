#pragma once

namespace Flux::DirectXConfig
{
	enum class ConstantBufferType
	{
		None,

		Unlit
	};

	enum class ShaderType
	{
		Unlit, // INFO: Both Vertex and Pixel Shader
		Skybox
	};

	enum class DepthWriteType
	{
		Enabled,
		Disabled
	};

	enum class CullingModeType
	{
		FrontSolid,
		BackSolid
	};
}

