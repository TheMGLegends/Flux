#include "FluxPCH.h"

#include "LayerManager.h"

namespace Flux
{
	LayerManager::LayerManager() : layerInsert(layers.begin())
	{
	}

	LayerManager::~LayerManager()
	{
		for (Layer* layer : layers)
		{
			FLUX_CORE_ASSERT(layer != nullptr, "LayerManager contains a null layer!");

			layer->OnDetach();
			delete layer;
		}

		layers.clear();
	}

	void LayerManager::OnEvent(Event& event)
	{
		for (auto it = layers.rbegin(); it != layers.rend(); ++it)
		{
			Layer* layer = *it;

			FLUX_CORE_ASSERT(layer != nullptr, "LayerManager contains a null layer!");

			if (layer->IsEnabled()) { layer->OnEvent(event); }
			if (event.IsHandled()) { break; }
		}
	}

	void LayerManager::Update()
	{
		for (Layer* layer : layers)
		{
			FLUX_CORE_ASSERT(layer != nullptr, "LayerManager contains a null layer!");

			if (layer->IsEnabled()) { layer->Update(); }
		}
	}

	void LayerManager::PushLayer(Layer* layer)
	{
		FLUX_CORE_ASSERT(layer != nullptr, "Trying to push a null layer to the LayerManager!");

		layer->OnAttach();
		layerInsert = layers.emplace(layerInsert, layer);
	}

	void LayerManager::PopLayer(Layer* layer)
	{
		FLUX_CORE_ASSERT(layer != nullptr, "Trying to pop a null layer from the LayerManager!");

		Layers::iterator layerEnd = std::next(layerInsert);
		auto it = std::find(layers.begin(), layerEnd, layer);

		if (it != layerEnd)
		{
			(*it)->OnDetach();
			layers.erase(it);
			--layerInsert;
		}
	}

	void LayerManager::PushOverlay(Layer* overlay)
	{
		FLUX_CORE_ASSERT(overlay, "Trying to push a null overlay to the LayerManager!");

		overlay->OnAttach();
		layers.emplace_back(overlay);
	}

	void LayerManager::PopOverlay(Layer* overlay)
	{
		FLUX_CORE_ASSERT(overlay, "Trying to pop a null overlay from the LayerManager!");

		Layers::iterator overlaysBegin = std::next(layerInsert);
		auto it = std::find(overlaysBegin, layers.end(), overlay);

		if (it != layers.end())
		{
			(*it)->OnDetach();
			layers.erase(it);
		}
	}

	void LayerManager::SetLayerEnabled(Layer* layer, bool enabled)
	{
		FLUX_CORE_ASSERT(layer != nullptr, "Trying to enable/disable a null layer in the LayerManager!");

		auto it = std::find(layers.begin(), layers.end(), layer);

		if (it != layers.end())
		{
			(*it)->SetEnabled(enabled);
		}
	}
}