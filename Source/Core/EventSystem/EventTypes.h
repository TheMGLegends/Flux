#pragma once

namespace Flux
{
	enum class EventType
	{
		None = 0,

		Quit,

		LoadScene,
		SaveScene,

		WindowResized,
		SceneViewResized,
		
		GameObjectRemoved,

		PlayModeExited,
	};
}

