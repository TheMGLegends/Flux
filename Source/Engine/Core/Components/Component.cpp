#include "Component.h"

Component::Component() : isActive(true), canHaveMultiple(false), isRemoveable(true), gameObject(nullptr)
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
