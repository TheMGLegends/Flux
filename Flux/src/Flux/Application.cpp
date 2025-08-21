#include "FluxPCH.h" // INFO: Precompiled Header

#include "Application.h"

// TEMP: Testing the window creation
#include <sfml/Window.hpp>

namespace Flux
{
	void Application::Run()
	{
		// TEMP: Testing the window creation
        sf::Window window(sf::VideoMode({ 800,600 }), "My Window");
		FLUX_CORE_INFO("Window created with size: {}x{}", window.getSize().x, window.getSize().y);

        while (window.isOpen())
        {
            while (const std::optional event = window.pollEvent())
            {
                if (event->is<sf::Event::Closed>())
                    window.close();
            }
        }
	}
}