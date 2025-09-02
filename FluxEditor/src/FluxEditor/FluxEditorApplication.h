#include <Flux.h>
#include <Sandbox/SandboxApplication.h>

#include <filesystem>
#include <sfml/Graphics/Image.hpp>
#include <sfml/Graphics/RenderWindow.hpp>

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
	FluxEditorApplication() 
	{
		// INFO: Adding Default Flux Icon to Editor Window
		auto& nativeWindow = GetWindow()->GetNativeWindow();
		const std::filesystem::path iconPath = "resources/FluxIcon.png";

		sf::Image icon;
		FLUX_CORE_VERIFY(icon.loadFromFile(iconPath), std::format("Failed to load window icon from path: {0}", iconPath.string()));
		nativeWindow->setIcon(icon.getSize(), icon.getPixelsPtr());

		PushLayer(new EditorLayer()); 
		PushLayer(new SandboxLayer()); 
	}
	virtual ~FluxEditorApplication() override = default;
};