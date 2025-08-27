#include <Flux.h>

class ExampleLayer : public Flux::Layer
{
public:
	ExampleLayer() : Layer("ExampleLayer") {}

	virtual void OnAttach() override
	{
		FLUX_INFO("ExampleLayer Attached!");
	}

	virtual void OnDetach() override
	{
		FLUX_INFO("ExampleLayer Detached!");
	}

	virtual void OnEvent(Flux::Event& event) override
	{
		FLUX_INFO(event);
	}

	virtual void Update() override
	{
		static bool once = true;

		if (once)
		{
			FLUX_INFO("ExampleLayer is actively updating!");
			once = false;
		}
	}
};

class SandboxApplication : public Flux::Application
{
public:
	SandboxApplication() { PushLayer(new ExampleLayer()); }
	virtual ~SandboxApplication() override = default;
};

std::unique_ptr<Flux::Application> Flux::CreateApplication()
{
	return std::make_unique<SandboxApplication>();
}