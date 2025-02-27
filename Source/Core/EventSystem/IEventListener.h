#pragma once

#include "EventTypes.h"
#include "Events/Event.h"

#include <memory>

class IEventListener
{
public:
	virtual ~IEventListener() {}
	virtual void OnNotify(EventType eventType, std::shared_ptr<Event> event) = 0;
};

