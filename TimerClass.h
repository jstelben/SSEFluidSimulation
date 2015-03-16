#pragma once

#include <Windows.h>

class TimerClass
{
public:
	TimerClass(void);
	~TimerClass(void);
	bool Initialize();
	void Frame();
	float GetTime();

private:
	INT64 Frequency;
	float TicksPerMs;
	INT64 StartTime;
	float FrameTime;
};

