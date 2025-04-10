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

		static float DeltaTime() { return static_cast<float>(deltaTime); }
		static float ElapsedTime() { return static_cast<float>(elapsedTime); }

		static void DecrementAccumulator();
		static float Alpha() { return static_cast<float>(alpha); }
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

