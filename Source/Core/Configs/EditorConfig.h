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
	inline TransformMode previousTransformMode = TransformMode::Pan; // INFO: Used when RMB is released to revert to previous transform mode

	inline void StorePreviousTransformMode()
	{
		previousTransformMode = currentTransformMode;
	}

	inline void RevertToPreviousTransformMode()
	{
		currentTransformMode = previousTransformMode;
	}
}

