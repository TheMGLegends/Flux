#pragma once

#include "Event.h"

#include <sstream>

#include "Flux/Math/Vector2.h"

namespace Flux
{
	class FLUX_API WindowCloseEvent : public Event
	{
	public:
		WindowCloseEvent() = default;

		EVENT_CLASS_TYPE(EventType::WindowClose);
		EVENT_CLASS_CATEGORY(EventCategory::Application);
	};

	class FLUX_API WindowResizeEvent : public Event
	{
	public:
		WindowResizeEvent(unsigned int width, unsigned int height) : width(width), height(height) {}

		inline unsigned int GetWidth() const { return width; }
		inline unsigned int GetHeight() const { return height; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowResizeEvent: " << width << ", " << height;
			return ss.str();
		}

		EVENT_CLASS_TYPE(EventType::WindowResize);
		EVENT_CLASS_CATEGORY(EventCategory::Application);

	private:
		unsigned int width;
		unsigned int height;
	};

	class FLUX_API WindowFocusEvent : public Event
	{
	public:
		WindowFocusEvent() = default;

		EVENT_CLASS_TYPE(EventType::WindowFocus);
		EVENT_CLASS_CATEGORY(EventCategory::Application);
	};

	class FLUX_API WindowLostFocusEvent : public Event
	{
	public:
		WindowLostFocusEvent() = default;

		EVENT_CLASS_TYPE(EventType::WindowLostFocus);
		EVENT_CLASS_CATEGORY(EventCategory::Application);
	};

	class FLUX_API WindowMovedEvent : public Event
	{
	public:
		WindowMovedEvent(const Vector2I& position) : position(position) {}

		inline const Vector2I& GetPosition() const { return position; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowMovedEvent: " << position;
			return ss.str();
		}

		EVENT_CLASS_TYPE(EventType::WindowMoved);
		EVENT_CLASS_CATEGORY(EventCategory::Application);

	private:
		Vector2I position;
	};
}