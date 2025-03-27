#pragma once

namespace Flux::EditorConfig
{
	enum class TransformMode
	{
		Pan,
		Translate,
		Rotate,
		Scale
	};

	inline float sceneViewWidth = 1920.0f;
	inline float sceneViewHeight = 1080.0;
	inline const float FONT_SIZE = 16.0f;

	inline TransformMode currentTransformMode = TransformMode::Pan;
}

