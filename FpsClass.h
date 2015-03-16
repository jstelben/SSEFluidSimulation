#pragma once
#pragma comment(lib, "winmm.lib")

#include <Windows.h>
#include <mmsystem.h>

class FpsClass
{
public:
	FpsClass(void);
	~FpsClass(void);
	void Initialize();
	void Frame();
	int GetFps();

private:
	int Fps, Count;
	unsigned long StartTime;
};

