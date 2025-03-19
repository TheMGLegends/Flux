#include "Time.h"

#include "Core/Configs/RuntimeConfig.h"
#include "Core/Configs/TimeConfig.h"

using namespace Flux;
using namespace std::chrono;

constexpr float MICRO_TO_SEC = 1000000.0f;

double Time::deltaTime = 0.0f;
double Time::elapsedTime = 0.0f;
double Time::accumulator = 0.0f;
unsigned int Time::frameCount = 0;

time_point<steady_clock> Time::currentTime;
time_point<steady_clock> Time::previousTime = steady_clock::now();

void Time::Tick()
{
	currentTime = steady_clock::now();

	deltaTime = duration_cast<microseconds>(currentTime - previousTime).count() / MICRO_TO_SEC;
	elapsedTime += deltaTime;

	if (RuntimeConfig::IsInPlayMode())
		accumulator += deltaTime;

	previousTime = currentTime;

	frameCount++;
}

bool Time::PerformPhysicsUpdate()
{
	if (accumulator >= TimeConfig::FIXED_DELTA_TIME)
	{
		accumulator -= TimeConfig::FIXED_DELTA_TIME;
		return true;
	}
	return false;
}
