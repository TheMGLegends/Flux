#pragma once

#include <memory>

#include "EventTypes.h"
#include "Events/Event.h"

namespace Flux
{
	class IEventListener
	{
	public:
		virtual ~IEventListener() {}
		virtual void OnNotify(EventType eventType, std::shared_ptr<Event> event) = 0;
	};
}

