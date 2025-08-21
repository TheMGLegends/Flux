#include "FluxPCH.h" // INFO: Precompiled Header

#include "Application.h"

#include <sfml/Window.hpp>

namespace Flux
{
	void Application::Run()
	{
        sf::Window window(sf::VideoMode({ 800,600 }), "My Window");
		FLUX_CORE_INFO("Window created with size: {}x{}", window.getSize().x, window.getSize().y);

        // run the program as long as the window is open
        while (window.isOpen())
        {
            // check all the window's events that were triggered since the last iteration of the loop
            while (const std::optional event = window.pollEvent())
            {
                // "close requested" event: we close the window
                if (event->is<sf::Event::Closed>())
                    window.close();
            }
        }
	}
}