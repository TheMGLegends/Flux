#pragma once

#include "Flux/Core.h"

#include <vector>

#include "Layer.h"

// TODO: Implement enabled logic for layers to determine if Update/OnEvent should be called
namespace Flux
{
	class FLUX_API LayerManager
	{
	public:
		using Layers = std::vector<Layer*>;

		LayerManager();
		~LayerManager();

		void PushLayer(Layer* layer);
		void PopLayer(Layer* layer);

		void PushOverlay(Layer* overlay);
		void PopOverlay(Layer* overlay);

	private:
		Layers layers;
		Layers::iterator layerInsert;
	};
}