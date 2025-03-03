#pragma once

namespace Flux::DirectXConfig
{
	enum class ConstantBufferType
	{
		None = 0,

		Unlit
	};

	enum class ShaderType
	{
		None = 0,

		Unlit, // INFO: Both Vertex and Pixel Shader
		Skybox
	};

	enum class DepthWriteType
	{
		None = 0,

		Enabled,
		Disabled
	};

	enum class CullingModeType
	{
		None = 0,

		FrontSolid,
		BackSolid
	};
}

