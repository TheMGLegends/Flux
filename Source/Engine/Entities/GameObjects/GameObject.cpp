#include "GameObject.h"

#include "Core/EventSystem/Events/GameObjectRemovedEvent.h"

#include "Engine/Entities/Components/Components.h"

namespace Flux
{
	int GameObject::gameObjectCounter = -1;

	GameObject::GameObject() : isActive(true), name("GameObject"), type("GameObject")
	{
		transform = AddComponent<Transform>(this);
	}

	GameObject::~GameObject() = default;

	void GameObject::Serialize(nlohmann::flux_json& json) const
	{
		// INFO: Serialize GameObject Data
		json["GameObjects"].push_back({
			{"Name", name},
			{"Type", type},
			{"IsActive", isActive},
			{"Components", nlohmann::json::array()} // INFO: JSON array to store all Components
			});

		// INFO: Get the last Component JSON array
		nlohmann::flux_json& gameObjectJson = json["GameObjects"].back();

		// INFO: Serialize each Component on the GameObject
		for (auto& component : components)
		{
			component->Serialize(gameObjectJson);
		}
	}

	void GameObject::Deserialize(const nlohmann::flux_json& json)
	{
		// INFO: Deserialize GameObject Data
		if (json.contains("Name"))
		{
			name = json["Name"].get<std::string>();
		}

		if (json.contains("Type"))
		{
			type = json["Type"].get<std::string>();
		}

		if (json.contains("IsActive"))
		{
			isActive = json["IsActive"].get<bool>();
		}

		if (json.contains("Components"))
		{
			// INFO: Find out how many components are on the GameObject
			size_t componentCount = json["Components"].size();

			// INFO: Deserialize each Component on the GameObject
			for (size_t i = 0; i < componentCount; i++)
			{
				// INFO: Retrieve the component data from the json file
				auto& componentData = json["Components"][i];

				// INFO: Create the Component based on the type
				auto componentType = static_cast<ComponentType>(componentData["ComponentType"].get<unsigned int>());

				std::weak_ptr<Component> component;

				switch (componentType)
				{
				case ComponentType::Transform:
					component = AddComponent<Transform>(this);
					break;
				case ComponentType::Camera:
					component = AddComponent<Camera>(this);
					break;
				case ComponentType::PhysicsBody:
					component = AddComponent<PhysicsBody>(this);
					break;
				case ComponentType::Visualizer:
					component = AddComponent<Visualizer>(this);
					break;
				case ComponentType::BoxCollider:
					component = AddComponent<BoxCollider>(this);
					break;
				case ComponentType::SphereCollider:
					component = AddComponent<SphereCollider>(this);
					break;
				case ComponentType::None:
				default:
					break;
				}

				if (!component.expired()) { component.lock()->Deserialize(componentData); }
			}
		}
	}

	void GameObject::SetIsActive(bool _isActive)
	{
		// INFO: Immediate change in active state
		isActive = _isActive;

		// INFO: Handle any OnEnable/OnDisable logic here
		if (isActive)
		{
			OnEnable();

			// INFO: Components Start
			for (const auto& component : components)
			{
				if (!component->IsActive()) { continue; }
				component->Start();
			}

			Start();
		}
		else
		{
			OnDisable();
		}
	}

	void GameObject::Destroy()
	{
		// INFO: Handle any OnDestroy logic here
		OnDestroy();

		EventDispatcher::QueueEvent(EventType::GameObjectRemoved, std::make_shared<GameObjectRemovedEvent>(this));

		SetIsActive(false);
	}

	void GameObject::SetName(std::string_view _name)
	{
		name = _name;
	}

	std::string& GameObject::GetName()
	{
		return name;
	}

	void GameObject::SetType(std::string_view _type)
	{
		type = _type;
	}

	std::unique_ptr<GameObject> GameObject::CreateGameObject(const std::string& typeName)
	{
		std::unique_ptr<GameObject> gameObject;

		// INFO: Default GameObject type creation
		if (typeName == "GameObject")
		{
			gameObjectCounter++;
			gameObject = std::make_unique<GameObject>();
			std::string gameObjectIdentifier = std::format("GameObject{}", gameObjectCounter);
			gameObject.get()->SetName(gameObjectIdentifier);
			gameObject.get()->SetType("GameObject");

			return gameObject;
		}

		if (auto it = gameObjectTypes.find(typeName); it != gameObjectTypes.end())
		{
			int count = gameObjectTypeCounters[typeName];
			gameObjectTypeCounters[typeName]++;

			gameObject = it->second();
			std::string gameObjectIdentifier = std::format("{}{}", typeName, count);
			gameObject.get()->SetName(gameObjectIdentifier);
			gameObject.get()->SetType(typeName);

			return gameObject;
		}

		return nullptr;
	}

	const std::unordered_map<std::string, std::function<std::unique_ptr<GameObject>()>, StringHasher, std::equal_to<>>& GameObject::GetGameObjectTypes()
	{
		return gameObjectTypes;
	}

	void GameObject::RegisterGameObjectType(const std::string& typeName, std::function<std::unique_ptr<GameObject>()> creator)
	{
		if (auto [data, result] = gameObjectTypes.try_emplace(typeName, std::move(creator)); !result)
		{
			Debug::LogError("GameObject::RegisterGameObjectType - GameObject type already registered: " + typeName);
			return;
		}

		gameObjectTypeCounters[typeName] = 0;
	}
}
