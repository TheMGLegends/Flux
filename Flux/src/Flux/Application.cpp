#include "FluxPCH.h" // INFO: Precompiled Header

#include "Application.h"

// TEMP: Testing the window creation
#include "Flux/Events/ApplicationEvent.h"
#include "Flux/Logging/Formatters/MultiLevelFormatter.h"
#include <sfml/Window.hpp>

namespace Flux
{
	void Application::Run()
	{
		// TEMP: Testing the window creation
        sf::Window window(sf::VideoMode({ 800,600 }), "My Window");

        WindowResizeEvent e(800, 600);
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowResizeEvent>(std::bind(&Application::OnWindowResizeEvent, this, std::placeholders::_1));

        while (window.isOpen())
        {
            while (const std::optional event = window.pollEvent())
            {
                if (event->is<sf::Event::Closed>())
                    window.close();
            }
        }
	}

    bool Application::OnWindowResizeEvent(WindowResizeEvent& e)
    {
		FLUX_CORE_WARN("Event received: {}", e);

        return false;
    }
}