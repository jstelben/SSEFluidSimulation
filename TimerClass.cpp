#include "TimerClass.h"


TimerClass::TimerClass(void)
{
}


TimerClass::~TimerClass(void)
{
}

bool TimerClass::Initialize()
{
	QueryPerformanceFrequency((LARGE_INTEGER*)&Frequency);
	if(Frequency == 0)
	{
		return false;
	}
	TicksPerMs = (float)(Frequency / 1000);
	QueryPerformanceCounter((LARGE_INTEGER*)&StartTime);
	return true;
}

void TimerClass::Frame()
{
	INT64 currentTime;
	float timeDifference;

	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);

	timeDifference = (float)(currentTime - StartTime);
	FrameTime = timeDifference / TicksPerMs;
	StartTime = currentTime;
}

float TimerClass::GetTime()
{
	return FrameTime;
}