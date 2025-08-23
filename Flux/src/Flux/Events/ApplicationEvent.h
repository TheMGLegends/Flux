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

	// TODO: Implement WindowFocusEvent, WindowLostFocusEvent, and WindowMovedEvent
}