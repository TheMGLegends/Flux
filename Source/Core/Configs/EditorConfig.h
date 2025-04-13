#pragma once

#include <ImGuizmo.h>

namespace Flux::EditorConfig
{
	extern float sceneViewWidth;
	extern float sceneViewHeight;

	extern const float FONT_SIZE;

	extern int currentTransformOperation;
	extern int previousTransformOperation;
	extern ImGuizmo::MODE transformMode;

	extern float translationSnapValue;
	extern float rotationSnapValue;
	extern float scaleSnapValue;

	extern bool isMouseOverSceneView;
	extern bool isSceneViewFocused;

	extern bool sceneNeedsSaving;

	void StorePreviousTransformOperation();
	void RevertToPreviousTransformOperation();
	void SwitchTransformMode();
}

