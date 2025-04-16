#pragma once

#include <chrono>

namespace Flux
{
	class Time
	{
	public:
		Time() = delete;
		~Time() = delete;
		Time(const Time&) = delete;
		Time& operator=(const Time&) = delete;

		static void Tick();
		static bool PerformPhysicsUpdate();

		static float DeltaTime();
		static float ElapsedTime();

		static void DecrementAccumulator();
		static float Alpha();
		static unsigned int GetFrameCount();

	private:
		static double deltaTime;
		static double elapsedTime;
		static double accumulator; // INFO: Used for fixed time step
		static double alpha; // INFO: Used for interpolation
		static unsigned int frameCount;

		static std::chrono::time_point<std::chrono::steady_clock> currentTime;
		static std::chrono::time_point<std::chrono::steady_clock> previousTime;
	};
}

