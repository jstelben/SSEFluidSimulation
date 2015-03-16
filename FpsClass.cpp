#include "FpsClass.h"


FpsClass::FpsClass(void)
{
}


FpsClass::~FpsClass(void)
{
}

void FpsClass::Initialize()
{
	Fps = 0;
	Count = 0;
	StartTime = timeGetTime();
}

void FpsClass::Frame()
{
	Count++;
	if(timeGetTime() >= (StartTime + 1000))
	{
		Fps = Count;
		Count = 0;
		StartTime = timeGetTime();
	}
}

int FpsClass::GetFps()
{
	return Fps;
}