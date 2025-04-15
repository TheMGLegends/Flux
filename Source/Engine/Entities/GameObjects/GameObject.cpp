#include "GameObject.h"

#include "Core/EventSystem/EventDispatcher.h"
#include "Core/EventSystem/Events/GameObjectRemovedEvent.h"

#include "Engine/Entities/Components/Components.h"

namespace Flux
{
	int GameObject::gameObjectCounter = -1;

	GameObject::GameObject() : isActive(true), name("GameObject"), id("GameObject"), type("GameObject")
	{
		transform = AddComponent<Transform>(this);
	}

	GameObject::~GameObject()
	{
	}

	void GameObject::Serialize(nlohmann::flux_json& json) const
	{
		// INFO: Serialize GameObject Data
		json["GameObjects"].push_back({
			{"Name", name},
			{"ID", id},
			{"Type", type},
			{"IsActive", isActive},
			{"Components", nlohmann::json::array()} // INFO: JSON array to store all Components
			});

		// INFO: Get the last Component JSON array
		nlohmann::flux_json& gameObjectJson = json["GameObjects"].back();

		// INFO: Serialize each Component on the GameObject
		for (size_t i = 0; i < components.size(); i++)
		{
			const std::shared_ptr<Component>& component = components[i];

			component->Serialize(gameObjectJson);
		}
	}

	void GameObject::Deserialize(const nlohmann::flux_json& json)
	{
		// INFO: Deserialize GameObject Data
		name = json["Name"].get<std::string>();
		id = json["ID"].get<std::string>();
		type = json["Type"].get<std::string>();
		isActive = json["IsActive"].get<bool>();

		// INFO: Find out how many components are on the GameObject
		size_t componentCount = json["Components"].size();

		// INFO: Deserialize each Component on the GameObject
		for (size_t i = 0; i < componentCount; i++)
		{
			// INFO: Retrieve the component data from the json file
			auto& componentData = json["Components"][i];

			// INFO: Create the Component based on the type
			ComponentType componentType = static_cast<ComponentType>(componentData["ComponentType"].get<unsigned int>());

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

	void GameObject::SetIsActive(bool _isActive)
	{
		// INFO: Immediate change in active state
		isActive = _isActive;

		// INFO: Handle any OnEnable/OnDisable logic here
		if (isActive)
		{
			OnEnable();

			// INFO: Components Start
			for (size_t i = 0; i < components.size(); i++)
			{
				std::shared_ptr<Component>& component = components[i];

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

	std::unique_ptr<GameObject> GameObject::CreateGameObject(const std::string& typeName)
	{
		std::unique_ptr<GameObject> gameObject;

		// INFO: Default GameObject type creation
		if (typeName == "GameObject")
		{
			gameObjectCounter++;
			gameObject = std::make_unique<GameObject>();
			gameObject.get()->SetName("GameObject" + std::to_string(gameObjectCounter));
			gameObject.get()->SetID("GameObject" + std::to_string(gameObjectCounter));
			gameObject.get()->SetType("GameObject");

			return gameObject;
		}

		auto it = gameObjectTypes.find(typeName);
		if (it != gameObjectTypes.end())
		{
			int count = gameObjectTypeCounters[typeName]++;
			gameObject = it->second();
			gameObject.get()->SetName(typeName + std::to_string(count));
			gameObject.get()->SetID(typeName + std::to_string(count));
			gameObject.get()->SetType(typeName);

			return gameObject;
		}

		return nullptr;
	}

	const std::unordered_map<std::string, std::function<std::unique_ptr<GameObject>()>>& GameObject::GetGameObjectTypes()
	{
		return gameObjectTypes;
	}

	void GameObject::RegisterGameObjectType(const std::string& typeName, std::function<std::unique_ptr<GameObject>()> creator)
	{
		auto result = gameObjectTypes.try_emplace(typeName, std::move(creator));

		if (!result.second)
		{
			Debug::LogError("GameObject::RegisterGameObjectType - GameObject type already registered: " + typeName);
			return;
		}

		gameObjectTypeCounters[typeName] = 0;
	}
}
