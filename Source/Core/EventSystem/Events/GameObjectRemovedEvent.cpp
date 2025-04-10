#include "GameObjectRemovedEvent.h"

namespace Flux
{
	GameObjectRemovedEvent::GameObjectRemovedEvent(GameObject* _gameObject) : gameObject(_gameObject)
	{
	}

	GameObjectRemovedEvent::~GameObjectRemovedEvent()
	{
	}
}
