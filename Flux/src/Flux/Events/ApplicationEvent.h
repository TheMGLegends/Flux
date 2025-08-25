#pragma once

#include "Event.h"

#include <sstream>

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
		WindowMovedEvent(int x, int y) : x(x), y(y) {}

		inline int GetX() const { return x; }
		inline int GetY() const { return y; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowMovedEvent: " << x << ", " << y;
			return ss.str();
		}

		EVENT_CLASS_TYPE(EventType::WindowMoved);
		EVENT_CLASS_CATEGORY(EventCategory::Application);

	private:
		int x;
		int y;
	};
}