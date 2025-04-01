#pragma once

#include <ImGuizmo.h>

namespace Flux::EditorConfig
{
	inline float sceneViewWidth = 1920.0f;
	inline float sceneViewHeight = 1080.0;
	inline const float FONT_SIZE = 18.0f;

	inline int currentTransformOperation = -1;
	inline int previousTransformOperation = -1;
	inline ImGuizmo::MODE transformMode = ImGuizmo::LOCAL;

	inline float translationSnapValue = 1.0f;
	inline float rotationSnapValue = 45.0f;
	inline float scaleSnapValue = 1.0f;

	inline void StorePreviousTransformOperation()
	{
		previousTransformOperation = currentTransformOperation;
	}

	inline void RevertToPreviousTransformOperation()
	{
		currentTransformOperation = previousTransformOperation;
	}

	inline void SwitchTransformMode()
	{
		if (transformMode == ImGuizmo::LOCAL)
		{
			transformMode = ImGuizmo::WORLD;
		}
		else
		{
			transformMode = ImGuizmo::LOCAL;
		}
	}
}

