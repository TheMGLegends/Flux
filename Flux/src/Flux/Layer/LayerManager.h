#pragma once

#include "Flux/Core.h"

#include "Flux/Events/Event.h"

#include <vector>

#include "Layer.h"

namespace Flux
{
	class FLUX_API LayerManager : public IEventListener
	{
	public:
		using Layers = std::vector<Layer*>;

		LayerManager();
		~LayerManager();

		virtual void OnEvent(Event& event) override;

		void Update();

		void PushLayer(Layer* layer);
		void PopLayer(Layer* layer);

		void PushOverlay(Layer* overlay);
		void PopOverlay(Layer* overlay);

		void SetLayerEnabled(Layer* layer, bool enabled);

	private:
		Layers layers;
		Layers::iterator layerInsert;
	};
}