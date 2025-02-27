#pragma once

#include "Runtime.h"

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
	// TODO: Engine Specific Members
};

