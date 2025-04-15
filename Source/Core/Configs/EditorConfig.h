#pragma once

#include <ImGuizmo.h>

namespace Flux::EditorConfig
{
	namespace Internal
	{
		// INFO: Scene View Dimensions
		extern float sceneViewWidth;
		extern float sceneViewHeight;

		// INFO: Transform Gizmo Properties
		extern int currentTransformOperation;
		extern int previousTransformOperation;
		extern ImGuizmo::MODE transformMode;

		extern float translationSnapValue;
		extern float rotationSnapValue;
		extern float scaleSnapValue;

		// INFO: Scene View States
		extern bool isMouseOverSceneView;
		extern bool isSceneViewFocused;
		extern bool sceneNeedsSaving;
	}

	extern const float FONT_SIZE;
	extern const int PAN;

	void SetSceneViewWidth(float _sceneViewWidth);
	float GetSceneViewWidth();

	void SetSceneViewHeight(float _sceneViewHeight);
	float GetSceneViewHeight();

	void SetCurretTransformOperation(int _currentTransformOperation);
	int GetCurrentTransformOperation();

	void StorePreviousTransformOperation();
	void RevertToPreviousTransformOperation();

	ImGuizmo::MODE GetTransformMode();
	void SwitchTransformMode();

	float GetTranslationSnapValue();
	float GetRotationSnapValue();
	float GetScaleSnapValue();

	void SetIsMouseOverSceneView(bool _isMouseOverSceneView);
	bool IsMouseOverSceneView();

	void SetIsSceneViewFocused(bool _isSceneViewFocused);
	bool IsSceneViewFocused();

	void SetSceneNeedsSaving(bool _sceneNeedsSaving);
	bool SceneNeedsSaving();
}

