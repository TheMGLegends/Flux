#include "EditorConfig.h"

namespace Flux::EditorConfig
{
	namespace Internal
	{
		float sceneViewWidth = 1920.0f;
		float sceneViewHeight = 1080.0f;

		int currentTransformOperation = PAN;
		int previousTransformOperation = PAN;
		ImGuizmo::MODE transformMode = ImGuizmo::LOCAL;

		float translationSnapValue = 1.0f;
		float rotationSnapValue = 45.0f;
		float scaleSnapValue = 1.0f;

		bool isMouseOverSceneView = false;
		bool isSceneViewFocused = false;
		bool sceneNeedsSaving = false;
	}

	const float FONT_SIZE = 18.0f;
	const int PAN = -1; // INFO: Pan Mode

	void SetSceneViewWidth(float _sceneViewWidth)
	{
		using namespace Internal;
		sceneViewWidth = _sceneViewWidth;
	}

	float GetSceneViewWidth()
	{
		using namespace Internal;
		return sceneViewWidth;
	}

	void SetSceneViewHeight(float _sceneViewHeight)
	{
		using namespace Internal;
		sceneViewHeight = _sceneViewHeight;
	}

	float GetSceneViewHeight()
	{
		using namespace Internal;
		return sceneViewHeight;
	}

	void SetCurretTransformOperation(int _currentTransformOperation)
	{
		using namespace Internal;
		currentTransformOperation = _currentTransformOperation;
	}

	int GetCurrentTransformOperation()
	{
		using namespace Internal;
		return currentTransformOperation;
	}

	void StorePreviousTransformOperation()
	{
		using namespace Internal;
		previousTransformOperation = currentTransformOperation;
	}

	void RevertToPreviousTransformOperation()
	{
		using namespace Internal;
		currentTransformOperation = previousTransformOperation;
	}

	ImGuizmo::MODE GetTransformMode()
	{
		using namespace Internal;
		return transformMode;
	}

	void SwitchTransformMode()
	{
		using namespace Internal;

		if (transformMode == ImGuizmo::LOCAL)
		{
			transformMode = ImGuizmo::WORLD;
		}
		else
		{
			transformMode = ImGuizmo::LOCAL;
		}
	}

	float GetTranslationSnapValue()
	{
		using namespace Internal;
		return translationSnapValue;
	}

	float GetRotationSnapValue()
	{
		using namespace Internal;
		return rotationSnapValue;
	}

	float GetScaleSnapValue()
	{
		using namespace Internal;
		return scaleSnapValue;
	}

	void SetIsMouseOverSceneView(bool _isMouseOverSceneView)
	{
		using namespace Internal;
		isMouseOverSceneView = _isMouseOverSceneView;
	}

	bool IsMouseOverSceneView()
	{
		using namespace Internal;
		return isMouseOverSceneView;
	}

	void SetIsSceneViewFocused(bool _isSceneViewFocused)
	{
		using namespace Internal;
		isSceneViewFocused = _isSceneViewFocused;
	}

	bool IsSceneViewFocused()
	{
		using namespace Internal;
		return isSceneViewFocused;
	}

	void SetSceneNeedsSaving(bool _sceneNeedsSaving)
	{
		using namespace Internal;
		sceneNeedsSaving = _sceneNeedsSaving;
	}

	bool SceneNeedsSaving()
	{
		using namespace Internal;
		return sceneNeedsSaving;
	}
}
