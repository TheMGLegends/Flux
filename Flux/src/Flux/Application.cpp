#include "FluxPCH.h"

#include "Application.h"

// TEMP: Testing the window creation
#include "Flux/Events/ApplicationEvent.h"
#include "Flux/Window/Window.h"
#include "Flux/Logging/Formatters/MultiLevelFormatter.h"
#include <sfml/Window.hpp>

namespace Flux
{
	void Application::Run()
	{
		// TEMP: Testing the window creation
        Window window;

        WindowResizeEvent e(800, 600);
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowResizeEvent>(std::bind(&Application::OnWindowResizeEvent, this, std::placeholders::_1));

		window.Update();
	}

    bool Application::OnWindowResizeEvent(WindowResizeEvent& e)
    {
		FLUX_CORE_WARN("Event received: {}", e);

        return false;
    }
}