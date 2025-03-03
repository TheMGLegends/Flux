#pragma once

#include "EventTypes.h"

#include <memory>
#include <queue>
#include <unordered_map>
#include <vector>

namespace Flux
{
	class IEventListener;
	class Event;

	class EventDispatcher
	{
	public:
		EventDispatcher();
		~EventDispatcher();

		/// @brief Prevents the same listener from being added to the same event type multiple times
		bool AddListener(EventType eventType, IEventListener* listener);
		/// @brief Removes a listener from all event types it is listening to
		void RemoveListener(IEventListener* listener);

		/// @brief Immediate notification of all listeners listening to the event type
		void Notify(EventType eventType, std::shared_ptr<Event> event);
		/// @brief Delayed notification of all listeners listening to the event type until ProcessEvents is called
		void QueueEvent(EventType eventType, std::shared_ptr<Event> event);

		/// @brief Processes all events in the event queue
		void ProcessEvents();

	private:
		std::unordered_map<EventType, std::vector<IEventListener*>> listeners;
		std::queue<std::pair<EventType, std::shared_ptr<Event>>> eventQueue;
	};
}

