#include "FluxPCH.h"

#include "LayerManager.h"

// TODO: Think about where OnAttach and OnDetach should be called

namespace Flux
{
	LayerManager::LayerManager() : layerInsert(layers.begin())
	{
	}

	LayerManager::~LayerManager()
	{
		for (Layer* layer : layers)
		{
			delete layer;
		}

		layers.clear();
	}

	void LayerManager::PushLayer(Layer* layer)
	{
		layerInsert = layers.insert(layerInsert, layer);
	}

	void LayerManager::PopLayer(Layer* layer)
	{
		Layers::iterator layerEnd = std::next(layerInsert);
		auto it = std::find(layers.begin(), layerEnd, layer);

		if (it != layerEnd)
		{
			layers.erase(it);
			--layerInsert;
		}
	}

	void LayerManager::PushOverlay(Layer* overlay)
	{
		layers.emplace_back(overlay);
	}

	void LayerManager::PopOverlay(Layer* overlay)
	{
		Layers::iterator overlaysBegin = std::next(layerInsert);
		auto it = std::find(overlaysBegin, layers.end(), overlay);

		if (it != layers.end())
		{
			layers.erase(it);
		}
	}
}