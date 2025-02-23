#pragma once

#include <chrono>

class Time
{
public:
	Time() = delete;
	~Time() = delete;
	Time(const Time&) = delete;
	Time& operator=(const Time&) = delete;

	static void Tick();
	static bool PerformPhysicsUpdate();

	static inline float DeltaTime() { return static_cast<float>(deltaTime); }
	static inline float ElapsedTime() { return static_cast<float>(elapsedTime); }

	static void SetFrameCount(const unsigned int _frameCount) { frameCount = _frameCount; }
	static inline unsigned int GetFrameCount() { return frameCount; }

private:
	static double deltaTime;
	static double elapsedTime;
	static double accumulator; // INFO: Used for fixed time step
	static unsigned int frameCount;

	static std::chrono::time_point<std::chrono::steady_clock> currentTime;
	static std::chrono::time_point<std::chrono::steady_clock> previousTime;
};

