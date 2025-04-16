#include "EventDispatcher.h"

#include "IEventListener.h"

#include "Core/GlobalDefines.h"

namespace Flux
{
	using namespace GlobalDefines;

	std::unordered_map<EventType, std::vector<IEventListener*>> EventDispatcher::listeners;
	std::queue<std::pair<EventType, std::shared_ptr<Event>>> EventDispatcher::eventQueue;

	int EventDispatcher::AddListener(EventType eventType, IEventListener* listener)
	{
		// INFO: Check if the listener is already listening to the event type
		if (auto it = listeners.find(eventType); it != listeners.end())
		{
			for (const IEventListener* existingListener : it->second)
			{
				if (existingListener == listener) { return FLUX_FAILURE; }
			}
		}

		listeners[eventType].push_back(listener);

		// INFO: Check if the listener was added successfully
		if (listeners[eventType].back() == listener) { return FLUX_SUCCESS; }

		return FLUX_FAILURE;
	}

	void EventDispatcher::RemoveListener(const IEventListener* listener)
	{
		for (auto it = listeners.begin(); it != listeners.end(); it++)
		{
			for (size_t i = 0; i < it->second.size(); i++)
			{
				if (it->second[i] != listener)
					continue;

				// INFO: Since the listener can only subscribe to each event type once, we can break out of the loop
				it->second.erase(it->second.begin() + i);
				break;
			}
		}
	}

	void EventDispatcher::Notify(EventType eventType, std::shared_ptr<Event> event)
	{
		if (auto it = listeners.find(eventType); it != listeners.end())
		{
			for (IEventListener* eventListener : it->second)
			{
				if (eventListener == nullptr) { continue; }

				eventListener->OnNotify(eventType, event);
			}
		}
	}

	void EventDispatcher::QueueEvent(EventType eventType, std::shared_ptr<Event> event)
	{
		eventQueue.emplace(eventType, std::move(event));
	}

	void EventDispatcher::ProcessEvents()
	{
		if (eventQueue.empty()) { return; }

		while (!eventQueue.empty())
		{
			const auto& [eventType, event] = eventQueue.front();
			Notify(eventType, event);
			eventQueue.pop();
		}
	}
}
