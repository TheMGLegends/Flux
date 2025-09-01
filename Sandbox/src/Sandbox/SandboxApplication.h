#include <Flux.h>

class SandboxLayer : public Flux::Layer
{
public:
	SandboxLayer() : Layer("SandboxLayer") {}

	virtual void OnAttach() override
	{
		FLUX_INFO("SandboxLayer Attached!");
	}

	virtual void OnDetach() override
	{
		FLUX_INFO("SandboxLayer Detached!");
	}

	virtual void OnEvent(Flux::Event& event) override
	{
		FLUX_INFO("Sandbox: {0}", event);
	}

	virtual void Update() override
	{
		static bool once = true;

		if (once)
		{
			FLUX_INFO("SandboxLayer is actively updating!");
			once = false;
		}
	}
};

class SandboxApplication : public Flux::Application
{
public:
	SandboxApplication() { PushLayer(new SandboxLayer()); }
	virtual ~SandboxApplication() override = default;
};