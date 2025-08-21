#pragma once

#include "Flux/Core.h"

#include <string>
#include <functional>

namespace Flux
{
	// TODO: Implement Deferred Event Dispatching (Currently Supports Immediate Dispatching)

	enum class EventType
	{
		None = 0,
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,	// Application Events
		KeyPressed, KeyReleased,												// Keyboard Events
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled		// Mouse Events
	};

	// TODO: Convert to Enum Class (And create custom operator overloads as needed)
	enum EventCategory
	{
		None = 0,
		EventCategoryApplication	= BIT(0),
		EventCategoryInput			= BIT(1),
		EventCategoryKeyboard		= BIT(2),
		EventCategoryMouse			= BIT(3),
		EventCategoryMouseButton	= BIT(4)
	};

#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::type; } \
							   virtual EventType GetEventType() const override { return GetStaticType(); } \
							   virtual const char* GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }

	// TODO: Think of what functions are Debug only (Not in Release)
	class FLUX_API Event
	{
		friend class EventDispatcher;
		
	public:
		virtual ~Event() = default;

		virtual EventType GetEventType() const = 0;
		virtual const char* GetName() const = 0;
		virtual int GetCategoryFlags() const = 0;
		virtual std::string ToString() const { return GetName(); }

		inline bool IsInCategory(EventCategory category) const
		{
			return GetCategoryFlags() & category;
		}

	protected:
		bool handled = false;
	};

	class EventDispatcher
	{
		template<typename T>
		using EventFunction = std::function<bool(T&)>;

	public:
		EventDispatcher(Event& event) : event(event) {}

		template<typename T>
		bool Dispatch(EventFunction<T> function)
		{
			// TODO: Maybe check to see if T is derived from Event?

			if (event.GetEventType() == T::GetStaticType())
			{
				T& actualEvent = static_cast<T&>(event);
				event.handled = function(actualEvent);
				return true;
			}
			
			return false;
		}

	private:
		Event& event;
	};

	// TODO: Doesn't work for spdlog, need to make a custom formatter for Event class
	inline std::ostream& operator<<(std::ostream& os, const Event& e)
	{
		return os << e.ToString();
	}
}