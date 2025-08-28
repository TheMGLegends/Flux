#pragma once

#include "Core.h"

#include <memory>

#include "Layer/LayerManager.h"
#include "Window/Window.h"

namespace Flux
{
	class WindowCloseEvent;

	class Application : public IEventListener
	{
	public:
		Application();
		virtual ~Application() = default;

		virtual void OnEvent(Event& event) override;

		void Run();

		void PushLayer(Layer* layer) { layerManager.PushLayer(layer); }
		void PushOverlay(Layer* overlay) { layerManager.PushOverlay(overlay); }

	private:
		bool OnWindowClose(WindowCloseEvent& event);

	private:
		bool isRunning;

		std::unique_ptr<Window> window;
		LayerManager layerManager;
	};

	// INFO: Defined by client to create custom entry point 
	std::unique_ptr<Application> CreateApplication();
}