#pragma once

#include "Event.h"

#include "Engine/Entities/GameObjects/GameObject.h"

namespace Flux
{
	class GameObjectRemovedEvent : public Event
	{
	public:
		GameObjectRemovedEvent(GameObject* _gameObject);
		virtual ~GameObjectRemovedEvent() override;

	public:
		GameObject* gameObject;
	};
}

