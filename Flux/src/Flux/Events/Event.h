#pragma once

#include "Flux/Core.h"

#include <string>
#include <format>
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

	enum class EventCategory
	{
		None			= 0,
		Application		= BIT(0),
		Input			= BIT(1),
		Keyboard		= BIT(2),
		Mouse			= BIT(3),
		MouseButton		= BIT(4)
	};

	inline int operator|(EventCategory a, EventCategory b)
	{
		return (static_cast<int>(a) | static_cast<int>(b));
	}

#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return type; } \
							   virtual EventType GetEventType() const override { return GetStaticType(); } \
							   virtual const char* GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return static_cast<int>(category); }

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
			return GetCategoryFlags() & static_cast<int>(category);
		}

	protected:
		bool handled = false;
	};

	class IOnEvent
	{
	public:
		virtual void OnEvent(Event& event) = 0;
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
}

// INFO: Specialization for Logging Event Derived Types
template<std::derived_from<Flux::Event> Derived, typename CharT>
struct std::formatter<Derived, CharT> : std::formatter<std::string>
{
	template <typename FormatContext>
	FormatContext::iterator format(const Derived& event, FormatContext& ctx) const 
	{
		return std::formatter<std::string>::format(event.ToString(), ctx);
	}
};