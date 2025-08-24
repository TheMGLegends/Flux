#include "FluxPCH.h"

#include "Window.h"

#include <sfml/Graphics/Image.hpp>
#include <sfml/Graphics/RenderWindow.hpp>

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
		// TEMP: Testing the window creation
		while (window->isOpen())
		{
			while (const std::optional event = window->pollEvent())
			{
				if (event->is<sf::Event::Closed>())
					window->close();
			}
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
