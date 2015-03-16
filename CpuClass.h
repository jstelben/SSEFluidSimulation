#pragma once
#pragma comment(lib, "pdh.lib")

#include <Pdh.h>

class CpuClass
{
public:
	CpuClass(void);
	~CpuClass(void);
	void Initialize();
	void Shutdown();
	void Frame();
	int GetCpuPercentage();

private:
	bool CanReadCpu;
	HQUERY QueryHandle;
	HCOUNTER CounterHandle;
	unsigned long LastSampleTime;
	long CpuUsage;
};

