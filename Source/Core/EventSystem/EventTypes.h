#pragma once

namespace Flux
{
	enum class EventType
	{
		None = 0,

		Quit,
		GameObjectRemoved,
		WindowResized,
		SceneViewResized,
		PlayModeExited,
		SceneSaved
	};
}

