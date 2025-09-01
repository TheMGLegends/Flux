#include <Flux.h>
#include <Sandbox/SandboxApplication.h>

class EditorLayer : public Flux::Layer
{
public:
	EditorLayer() : Layer("EditorLayer") {}

	virtual void OnAttach() override
	{
		FLUX_INFO("EditorLayer Attached!");
	}

	virtual void OnDetach() override
	{
		FLUX_INFO("EditorLayer Detached!");
	}

	virtual void OnEvent(Flux::Event& event) override
	{
		FLUX_INFO("FluxEditor: {0}", event);
	}

	virtual void Update() override
	{
		static bool once = true;

		if (once)
		{
			FLUX_INFO("EditorLayer is actively updating!");
			once = false;
		}
	}
};

class FluxEditorApplication : public Flux::Application
{
public:
	FluxEditorApplication() { PushLayer(new EditorLayer()); PushLayer(new SandboxLayer()); }
	virtual ~FluxEditorApplication() override = default;
};