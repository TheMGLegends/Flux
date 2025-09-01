#pragma once

#include "Flux/Core.h"

#include "Flux/Events/Event.h"

#include <vector>

#include "Layer.h"

namespace Flux
{
	class LayerManager : public IEventListener
	{
	public:
		using Layers = std::vector<Layer*>;

		LayerManager();
		~LayerManager();

		virtual void OnEvent(Event& event) override;

		void Update();

		/// <summary>
		/// Push Layers in the order in which you want them to receive events (First Push = First to Receive Events)
		/// </summary>
		void PushLayer(Layer* layer);
		void PopLayer(Layer* layer);

		/// <summary>
		/// Push Overlays in the order in which you want them to receive events (Last Push = First to Receive Events)
		/// </summary>
		void PushOverlay(Layer* overlay);
		void PopOverlay(Layer* overlay);

		void SetLayerEnabled(Layer* layer, bool enabled);

	private:
		Layers layers;
		Layers::iterator layerInsert;
	};
}