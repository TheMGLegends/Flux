#include <Flux.h>

class SandboxApplication : public Flux::Application
{
public:
	SandboxApplication() = default;
	virtual ~SandboxApplication() override = default;
};

Flux::Application* Flux::CreateApplication()
{
	return new SandboxApplication();
}