#pragma once

namespace Flux
{
	enum class EventType
	{
		None = 0,

		Quit,
		ComponentRemoved,
		GameObjectRemoved,
		WindowResized,
		SceneViewResized,
		PlayModeExited
	};
}

