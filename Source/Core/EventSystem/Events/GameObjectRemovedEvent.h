#pragma once

#include "Event.h"

namespace Flux
{
	class GameObject;

	class GameObjectRemovedEvent : public Event
	{
	public:
		GameObjectRemovedEvent(GameObject* _gameObject) : gameObject(_gameObject) {}
		virtual ~GameObjectRemovedEvent() override {}

	public:
		GameObject* gameObject;
	};
}

