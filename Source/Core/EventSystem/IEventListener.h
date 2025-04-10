#pragma once

#include <memory>

#include "EventTypes.h"

namespace Flux
{
	class Event;

	class IEventListener
	{
	public:
		virtual ~IEventListener() = default;
		virtual void OnNotify(EventType eventType, std::shared_ptr<Event> event) = 0;
	};
}

