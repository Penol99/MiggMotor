#include "Timer.h"


Timer::Timer()
	:myLastFrame(std::chrono::high_resolution_clock::now()),
	myCurrentFrame(std::chrono::high_resolution_clock::now()),
	myTotaltTime(0.0f),
	myDeltaTime(0.0f),
	myFPSTime(0.0),
	myFPSCounter(0),
	myFPS(0)
{
}

void Timer::Update()
{
	myLastFrame = myCurrentFrame;
	myCurrentFrame = std::chrono::high_resolution_clock::now();

	std::chrono::duration<float> delta = myCurrentFrame - myLastFrame;
	myDeltaTime = delta.count();
	myTotaltTime += myDeltaTime;
	myFPSTime += myDeltaTime;
	
	++myFPSCounter;
	if (myFPSTime >= 1.0f)
	{
		myFPSTime -= 1.0f;
		myFPS = (int)myFPSCounter;
		myFPSCounter = 0;
	}
}

float Timer::GetDeltaTime() const
{ 
	return myDeltaTime; 
}

double Timer::GetTotalTime() const
{ 
	return static_cast<double>(myTotaltTime); 
}

int Timer::GetFPS() const
{ 
	return myFPS;
}

