#pragma once

namespace Flux
{
	enum class EventType
	{
		None = 0,

		Quit,

		LoadScene,
		SaveScene,
		CreateScene,

		WindowResized,
		SceneViewResized,
		
		GameObjectRemoved,
		ComponentRemoved,

		PlayModeExited,
	};
}

