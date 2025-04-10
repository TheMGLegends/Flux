#pragma once

namespace Flux::DirectXConfig
{
	enum class ConstantBufferType
	{
		None = 0,

		Unlit
	};

	/// @brief Represents Vertex and Pixel Shader Types
	enum class ShaderType
	{
		None = 0,

		Unlit,
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

