#pragma once

#include "Event.h"

#include <memory>

class Component;

class ComponentRemovedEvent : public Event
{
public:
	ComponentRemovedEvent(std::weak_ptr<Component> _component) : component(_component) {}
	virtual ~ComponentRemovedEvent() override {}

public:
	std::weak_ptr<Component> component;
};

