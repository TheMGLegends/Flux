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
		for (size_t i = 0; i < listeners[eventType].size(); i++)
		{
			if (listeners[eventType][i] == listener) { return FLUX_FAILURE; }
		}

		listeners[eventType].push_back(listener);

		// INFO: Check if the listener was added successfully
		if (listeners[eventType].back() == listener) { return FLUX_SUCCESS; }

		return FLUX_FAILURE;
	}

	void EventDispatcher::RemoveListener(IEventListener* listener)
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
		for (size_t i = 0; i < listeners[eventType].size(); i++)
		{
			listeners[eventType][i]->OnNotify(eventType, event);
		}
	}

	void EventDispatcher::QueueEvent(EventType eventType, std::shared_ptr<Event> event)
	{
		eventQueue.push(std::make_pair(eventType, std::move(event)));
	}

	void EventDispatcher::ProcessEvents()
	{
		if (eventQueue.empty()) { return; }

		while (!eventQueue.empty())
		{
			auto& event = eventQueue.front();
			Notify(event.first, event.second);
			eventQueue.pop();
		}
	}
}
