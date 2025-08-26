#pragma once

#include "Event.h"

#include <sstream>

namespace Flux
{
	class FLUX_API KeyEvent : public Event
	{
	public:
		inline int GetKeyCode() const { return keyCode; }

		EVENT_CLASS_CATEGORY(EventCategory::Keyboard | EventCategory::Input);

	protected:
		KeyEvent(int keyCode) : keyCode(keyCode) {}

		int keyCode;
	};

	class FLUX_API KeyPressedEvent : public KeyEvent
	{
	public:
		KeyPressedEvent(int keyCode, int repeatCount) : KeyEvent(keyCode), repeatCount(repeatCount) {}

		inline int GetRepeatCount() const { return repeatCount; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyPressedEvent: " << keyCode << " (Repeat Count: " << repeatCount << ")";
			return ss.str();
		}

		EVENT_CLASS_TYPE(EventType::KeyPressed);

	private:
		int repeatCount;
	};

	class FLUX_API KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(int keyCode) : KeyEvent(keyCode) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyReleasedEvent: " << keyCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(EventType::KeyReleased);
	};
}