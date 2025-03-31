#pragma once

#include <ImGuizmo.h>

namespace Flux::EditorConfig
{
	inline float sceneViewWidth = 1920.0f;
	inline float sceneViewHeight = 1080.0;
	inline const float FONT_SIZE = 16.0f;

	inline ImGuizmo::OPERATION transformOperation = ImGuizmo::TRANSLATE;
	inline ImGuizmo::MODE transformMode = ImGuizmo::LOCAL;
	inline bool isPanning = false;

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

