#include <Flux.h>

class SandboxApplication : public Flux::Application
{
public:
	SandboxApplication() = default;
	virtual ~SandboxApplication() override = default;
};

std::unique_ptr<Flux::Application> Flux::CreateApplication()
{
	return std::make_unique<SandboxApplication>();
}