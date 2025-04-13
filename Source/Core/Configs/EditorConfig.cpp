#include "EditorConfig.h"

namespace Flux::EditorConfig
{
	float sceneViewWidth = 1920.0f;
	float sceneViewHeight = 1080.0f;

	const float FONT_SIZE = 18.0f;

	int currentTransformOperation = -1; // -1 (Pan)
	int previousTransformOperation = -1; // -1 (Pan)
	ImGuizmo::MODE transformMode = ImGuizmo::LOCAL;

	float translationSnapValue = 1.0f;
	float rotationSnapValue = 45.0f;
	float scaleSnapValue = 1.0f;

	bool isMouseOverSceneView = false;
	bool isSceneViewFocused = false;

	bool sceneNeedsSaving = false;

	void StorePreviousTransformOperation()
	{
		previousTransformOperation = currentTransformOperation;
	}

	void RevertToPreviousTransformOperation()
	{
		currentTransformOperation = previousTransformOperation;
	}

	void SwitchTransformMode()
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
