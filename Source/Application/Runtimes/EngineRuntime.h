#pragma once

#include "Runtime.h"

#include <memory>

class Scene;

class EngineRuntime : public Runtime
{
public:
	EngineRuntime(EventDispatcher& _eventDispatcher);
	virtual ~EngineRuntime() override;

	bool PreInitialise() override;
	bool Initialise() override;
	void Update(float deltaTime) override;
	void Release() override;

	void Start();
	void FixedUpate(float fixedDeltaTime);

private:
	std::unique_ptr<Scene> scene;
	// TODO: Engine Specific Members
};

