#include "CpuClass.h"


CpuClass::CpuClass(void)
{
}


CpuClass::~CpuClass(void)
{
}

void CpuClass::Initialize()
{
	PDH_STATUS status;

	CanReadCpu = true;
	status = PdhOpenQuery(NULL, 0, &QueryHandle);
	if(status != ERROR_SUCCESS)
	{
		CanReadCpu = false;
	}

	status = PdhAddCounter(QueryHandle, TEXT("\\Processor(_Total)\\% processor time"), 0, &CounterHandle);
	if(status != ERROR_SUCCESS)
	{
		CanReadCpu = false;
	}
	LastSampleTime = GetTickCount();
	CpuUsage = 0;
}

void CpuClass::Shutdown()
{
	if(CanReadCpu)
	{
		PdhCloseQuery(QueryHandle);
	}
}

void CpuClass::Frame()
{
	PDH_FMT_COUNTERVALUE value;

	if(CanReadCpu)
	{
		if((LastSampleTime + 1000) < GetTickCount())
		{
			LastSampleTime = GetTickCount();
			PdhCollectQueryData(QueryHandle);
			PdhGetFormattedCounterValue(CounterHandle, PDH_FMT_LONG, NULL, &value);
			CpuUsage = value.longValue;
		}
	}
}

int CpuClass::GetCpuPercentage()
{
	int usage;
	if(CanReadCpu)
	{
		usage = (int)CpuUsage;
	}
	else
	{
		usage = 0;
	}

	return usage;
}