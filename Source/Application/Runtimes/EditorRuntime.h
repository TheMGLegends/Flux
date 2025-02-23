#pragma once

#include "Runtime.h"

class EditorRuntime : public Runtime
{
public:
	EditorRuntime(EventDispatcher& _eventDispatcher);
	virtual ~EditorRuntime() override;

	bool PreInitialise() override;
	bool Initialise() override;
	void Update(float deltaTime) override;
	void Render() override;
	void Release() override;

private:
	// TODO: Editor Specific Members
};

