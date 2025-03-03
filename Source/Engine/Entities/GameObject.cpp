#include "GameObject.h"

using namespace Flux;

GameObject::GameObject() : isActive(true), eventDispatcher(nullptr)
{
	transform = AddComponent<Transform>();
}

GameObject::~GameObject()
{
}

void GameObject::Serialize(nlohmann::ordered_json& json) const
{
}

void GameObject::Deserialize(const nlohmann::ordered_json& json)
{
}

void GameObject::SetIsActive(bool _isActive)
{
	// INFO: Immediate change in active state
	isActive = _isActive;

	// INFO: Handle any OnEnable/OnDisable logic here
	if (isActive)
	{
		OnEnable();
		// TODO: Notify Event System of recent enabling of GO, so that it's start can be called at the start of the next frame
	}
	else
		OnDisable();
}

void GameObject::Destroy()
{
	// INFO: Handle any OnDestroy logic here
	OnDestroy();

	// TODO: Pass Destroy Event Info onto Event System so GOs are destroyed at the end of the frame (The listener will likely be the scene)

	SetIsActive(false);
}
