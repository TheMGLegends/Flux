#include "Runtime.h"

using namespace Flux;

Runtime::Runtime(EventDispatcher& _eventDispatcher) : eventDispatcher(_eventDispatcher)
{
}

Runtime::~Runtime()
{
}
