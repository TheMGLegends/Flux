#pragma once

#include <memory>
#include <queue>
#include <unordered_map>
#include <vector>

#include "EventTypes.h"

namespace Flux
{
	class IEventListener;
	class Event;

	class EventDispatcher
	{
	public:
		EventDispatcher() = delete;
		~EventDispatcher() = delete;
		EventDispatcher(const EventDispatcher&) = delete;
		EventDispatcher& operator=(const EventDispatcher&) = delete;

		/// @brief Prevents the same listener from being added to the same event type multiple times
		static bool AddListener(EventType eventType, IEventListener* listener);
		/// @brief Removes a listener from all event types it is listening to
		static void RemoveListener(IEventListener* listener);

		/// @brief Immediate notification of all listeners listening to the event type
		static void Notify(EventType eventType, std::shared_ptr<Event> event);
		/// @brief Delayed notification of all listeners listening to the event type until ProcessEvents is called
		static void QueueEvent(EventType eventType, std::shared_ptr<Event> event);

		/// @brief Processes all events in the event queue
		static void ProcessEvents();

	private:
		static std::unordered_map<EventType, std::vector<IEventListener*>> listeners;
		static std::queue<std::pair<EventType, std::shared_ptr<Event>>> eventQueue;
	};
}

