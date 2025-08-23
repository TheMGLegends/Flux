#pragma once

#include "Event.h"

#include <sstream>

namespace Flux
{
	class FLUX_API MouseButtonEvent : public Event
	{
	public:
		inline int GetButtonCode() const { return buttonCode; }

		EVENT_CLASS_CATEGORY(EventCategory::MouseButton | EventCategory::Input);

	protected:
		MouseButtonEvent(int buttonCode) : buttonCode(buttonCode) {}

		int buttonCode;
	};

	class FLUX_API MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonPressedEvent(int buttonCode) : MouseButtonEvent(buttonCode) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonPressedEvent: " << buttonCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(EventType::MouseButtonPressed);
	};

	class FLUX_API MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonReleasedEvent(int buttonCode) : MouseButtonEvent(buttonCode) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonReleasedEvent: " << buttonCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(EventType::MouseButtonReleased);
	};

	class FLUX_API MouseMovedEvent : public Event
	{
	public:
		MouseMovedEvent(float x, float y) : mouseX(x), mouseY(y) {}

		inline float GetX() const { return mouseX; }
		inline float GetY() const { return mouseY; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseMovedEvent: (" << mouseX << ", " << mouseY << ")";
			return ss.str();
		}

		EVENT_CLASS_TYPE(EventType::MouseMoved);
		EVENT_CLASS_CATEGORY(EventCategory::Mouse | EventCategory::Input);

	private:
		float mouseX;
		float mouseY;
	};

	class FLUX_API MouseScrolledEvent : public Event
	{
	public:
		MouseScrolledEvent(float offsetX, float offsetY) : offsetX(offsetX), offsetY(offsetY) {}

		inline float GetOffsetX() const { return offsetX; }
		inline float GetOffsetY() const { return offsetY; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseScrolledEvent: (" << offsetX << ", " << offsetY << ")";
			return ss.str();
		}

		EVENT_CLASS_TYPE(EventType::MouseScrolled);
		EVENT_CLASS_CATEGORY(EventCategory::Mouse | EventCategory::Input);

	private:
		float offsetX;
		float offsetY;
	};
}