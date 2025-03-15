#include "Component.h"

using namespace Flux;

Component::Component(GameObject* _gameObject) : isActive(true), canHaveMultiple(false), isRemoveable(true), gameObject(_gameObject), 
										        name(""), componentType(ComponentType::None)
{
}

Component::~Component()
{
}

void Component::Serialize(nlohmann::ordered_json& json) const
{
	// INFO: Serialize Component Data
	json["Components"].push_back({
		{"Name", name},
		{"IsActive", isActive},
		{"ComponentType", componentType}
		});
}

void Component::Deserialize(const nlohmann::ordered_json& json)
{
	// INFO: Deserialize Component Data
	name = json["Name"].get<std::string>();
	isActive = json["IsActive"].get<bool>();
}
