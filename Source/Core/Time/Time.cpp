#include "Time.h"

#include "Core/Configs/RuntimeConfig.h"
#include "Core/Configs/TimeConfig.h"

using namespace std::chrono;

namespace Flux
{
	constexpr float MICRO_TO_SEC = 1000000.0f;

	double Time::deltaTime = 0.0f;
	double Time::elapsedTime = 0.0f;
	double Time::accumulator = 0.0f;
	double Time::alpha = 0.0f;
	unsigned int Time::frameCount = 0;

	time_point<steady_clock> Time::currentTime;
	time_point<steady_clock> Time::previousTime = steady_clock::now();

	void Time::Tick()
	{
		currentTime = steady_clock::now();

		deltaTime = static_cast<double>(duration_cast<microseconds>(currentTime - previousTime).count()) / MICRO_TO_SEC;
		elapsedTime += deltaTime;

		if (RuntimeConfig::IsInPlayMode() && !RuntimeConfig::IsPaused())
		{
			accumulator += deltaTime;
			alpha = accumulator / TimeConfig::FIXED_DELTA_TIME;
		}

		previousTime = currentTime;

		frameCount++;
	}

	bool Time::PerformPhysicsUpdate()
	{
		if (accumulator >= TimeConfig::FIXED_DELTA_TIME) { return true; }

		return false;
	}

	float Time::DeltaTime()
	{
		return static_cast<float>(deltaTime);
	}

	float Time::ElapsedTime()
	{
		return static_cast<float>(elapsedTime);
	}

	void Time::DecrementAccumulator()
	{
		accumulator -= static_cast<double>(TimeConfig::FIXED_DELTA_TIME);
	}

	float Time::Alpha()
	{
		return static_cast<float>(alpha);
	}

	unsigned int Time::GetFrameCount()
	{
		// INFO: Copy over frame count and reset it for the next frame
		unsigned int count = frameCount;
		frameCount = 0;

		return count;
	}
}
