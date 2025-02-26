#pragma once

// TODO: EventDispatcher Include
class EventDispatcher;

class Runtime
{
public:
	Runtime(EventDispatcher& _eventDispatcher);
	virtual ~Runtime() = 0;

	virtual bool PreInitialise() = 0;
	virtual bool Initialise() = 0;
	virtual void Update(float deltaTime) {}
	virtual void Release() {}

protected:
	//EventDispatcher& eventDispatcher;
};

