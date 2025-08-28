#pragma once

#include "Event.h"

#include <sstream>

#include "Flux/Math/Vector2.h"

namespace Flux
{
	class MouseButtonEvent : public Event
	{
	public:
		int GetButtonCode() const { return buttonCode; }
		const Vector2I& GetPosition() const { return position; }

		EVENT_CLASS_CATEGORY(EventCategory::MouseButton | EventCategory::Input);

	protected:
		MouseButtonEvent(int buttonCode, const Vector2I& position) : buttonCode(buttonCode), position(position) {}

		int buttonCode;
		Vector2I position;
	};

	class MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonPressedEvent(int buttonCode, const Vector2I& position) : MouseButtonEvent(buttonCode, position) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonPressedEvent: " << buttonCode << " at " << position;
			return ss.str();
		}

		EVENT_CLASS_TYPE(EventType::MouseButtonPressed);
	};

	class MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonReleasedEvent(int buttonCode, const Vector2I& position) : MouseButtonEvent(buttonCode, position) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonReleasedEvent: " << buttonCode << " at " << position;
			return ss.str();
		}

		EVENT_CLASS_TYPE(EventType::MouseButtonReleased);
	};

	class MouseMovedEvent : public Event
	{
	public:
		MouseMovedEvent(const Vector2I& position) : position(position) {}

		const Vector2I& GetPosition() const { return position; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseMovedEvent: " << position;
			return ss.str();
		}

		EVENT_CLASS_TYPE(EventType::MouseMoved);
		EVENT_CLASS_CATEGORY(EventCategory::Mouse | EventCategory::Input);

	private:
		Vector2I position;
	};

	class MouseScrolledEvent : public MouseButtonEvent
	{
	public:
		MouseScrolledEvent(int buttonCode, const Vector2I& position, float delta) : MouseButtonEvent(buttonCode, position), delta(delta) {}

		float GetDelta() const { return delta; }

		std::string ToString() const override
		{
			std::stringstream ss;

			// INFO: Vertical Scroll
			if (buttonCode == 0)
			{
				ss << "MouseScrolledEvent: " << std::format("Vertical Scroll: {0} at {1}", delta, position);
			}
			// INFO: Horizontal Scroll
			else if (buttonCode == 1)
			{
				ss << "MouseScrolledEvent: " << std::format("Horizontal Scroll: {0} at {1}", delta, position);
			}

			return ss.str();
		}

		EVENT_CLASS_TYPE(EventType::MouseWheelScrolled);
		EVENT_CLASS_CATEGORY(EventCategory::Mouse | EventCategory::Input);

	private:
		float delta;
	};
}