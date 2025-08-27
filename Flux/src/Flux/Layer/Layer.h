#pragma once

#include "Flux/Core.h"

#include "Flux/Events/Event.h"

namespace Flux
{
	class FLUX_API Layer : public IEventListener
	{
	public:
		Layer(const std::string& name = "Layer") : name(name) {}
		virtual ~Layer() = default;

		virtual void OnEvent(Event& event) override {}

		virtual void OnAttach() {}
		virtual void OnDetach() {}

		virtual void Update() {}

		const std::string& GetName() const { return name; }

	protected:
		std::string name;
	};
}