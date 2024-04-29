#pragma once
#include <chrono>

class Timer
{
public:
	Timer();

	Timer(const Timer& aTimer) = delete;

	Timer& operator=(const Timer& aTimer) = delete;
	void Update();
	float GetDeltaTime() const;
	double GetTotalTime() const;
	int GetFPS() const;

private:
	std::chrono::high_resolution_clock::time_point myLastFrame;
	std::chrono::high_resolution_clock::time_point myCurrentFrame;
	float myTotaltTime;
	float myFPSTime;
	float myDeltaTime;
	float myFPSCounter;
	int myFPS;
};


