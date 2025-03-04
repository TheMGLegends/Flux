#include "Component.h"

using namespace Flux;

Component::Component(GameObject* _gameObject) : isActive(true), canHaveMultiple(false), isRemoveable(true), gameObject(_gameObject)
{
}

Component::~Component()
{
}

void Component::Serialize(nlohmann::ordered_json& json) const
{
	// TODO: Serialize Component
}

void Component::Deserialize(const nlohmann::ordered_json& json)
{
	// TODO: Deserialize Component
}
