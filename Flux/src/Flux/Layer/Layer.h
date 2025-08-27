#pragma once

#include "Flux/Core.h"

#include "Flux/Events/Event.h"

namespace Flux
{
	class FLUX_API Layer : public IEventListener
	{
	public:
		Layer(const std::string& name = "Layer", bool enabled = true) : name(name), enabled(enabled) {}
		virtual ~Layer() = default;

		virtual void OnEvent(Event& event) override {}

		virtual void OnAttach() {}
		virtual void OnDetach() {}

		virtual void Update() {}

		const std::string& GetName() const { return name; }

		bool IsEnabled() const { return enabled; }
		void SetEnabled(bool enabled) { this->enabled = enabled; }

	protected:
		std::string name;

		bool enabled;
	};
}