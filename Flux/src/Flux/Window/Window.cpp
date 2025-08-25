#include "FluxPCH.h"

#include "Window.h"

#include <sfml/Graphics/Image.hpp>
#include <sfml/Graphics/RenderWindow.hpp>

#include "Flux/Events/ApplicationEvent.h"
#include "Flux/Events/KeyEvent.h"
#include "Flux/Events/MouseEvent.h"

namespace Flux
{
	Window::Window(const WindowProperties& properties)
	{
		data.properties = properties;

		FLUX_CORE_INFO("Creating Window: {0}", data.properties);

		// INFO: SFML Window Creation
		window = std::make_unique<sf::RenderWindow>(sf::VideoMode({ data.properties.width, data.properties.height }), data.properties.title);
		FLUX_CORE_ASSERT(window != nullptr, "Failed to create SFML Window!");

		SetVSyncEnabled(true);
		data.properties.position = window->getPosition();

		// INFO: Adding Window Icon
		const std::filesystem::path iconPath = "resources/FluxIcon.png";
		sf::Image icon;
		FLUX_CORE_VERIFY(icon.loadFromFile(iconPath), std::format("Failed to load window icon from path: {0}", iconPath.string()));

		window->setIcon(icon.getSize(), icon.getPixelsPtr());
	}

	Window::~Window()
	{
		window->close();
	}

	void Window::Update()
	{
		while (const std::optional event = window->pollEvent())
		{
#pragma region ApplicationEvents
			// INFO: Window Close Event
			if (event->is<sf::Event::Closed>())
			{
				WindowCloseEvent closeEvent;
				eventCallback(closeEvent);
			}
			// INFO: Window Resize Event
			else if (const auto* windowResized = event->getIf<sf::Event::Resized>())
			{
				sf::Vector2u size = windowResized->size;
				data.properties.width = size.x;
				data.properties.height = size.y;

				WindowResizeEvent resizeEvent(data.properties.width, data.properties.height);
				eventCallback(resizeEvent);
			}
			// INFO: Window Focus Event
			else if (event->is<sf::Event::FocusGained>())
			{
				WindowFocusEvent focusEvent;
				eventCallback(focusEvent);
			}
			// INFO: Window Lost Focus Event
			else if (event->is<sf::Event::FocusLost>())
			{
				WindowLostFocusEvent lostFocusEvent;
				eventCallback(lostFocusEvent);
			}
			// INFO: Window Moved Event
			else if (sf::Vector2i windowPosition = window->getPosition(); windowPosition != data.properties.position)
			{
				data.properties.position = windowPosition;
				WindowMovedEvent movedEvent(data.properties.position.x, data.properties.position.y);
				eventCallback(movedEvent);
			}
#pragma endregion ApplicationEvents

#pragma region KeyboardEvents
#pragma endregion KeyboardEvents

#pragma region MouseEvents
#pragma endregion MouseEvents
		}
	}

	void Window::SetVSyncEnabled(bool enabled)
	{
		data.vsyncEnabled = enabled;
		window->setVerticalSyncEnabled(enabled);
	}

	void Window::SetFramerateLimit(unsigned int limit)
	{
		if (data.vsyncEnabled)
		{
			FLUX_CORE_WARN("VSync is enabled, setting a frame rate limit has no effect!");
			return;
		}

		data.framerateLimit = limit;
		window->setFramerateLimit(limit);
	}
}
