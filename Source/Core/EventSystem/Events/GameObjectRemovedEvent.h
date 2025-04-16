#pragma once

#include "Event.h"

#include "Engine/Entities/GameObjects/GameObject.h"

namespace Flux
{
	class GameObjectRemovedEvent : public Event
	{
	public:
		explicit GameObjectRemovedEvent(GameObject* _gameObject);
		~GameObjectRemovedEvent() override;

	public:
		GameObject* gameObject;
	};
}

