#ifndef _CPUMONITOR_H_
#define _CPUMONITOR_H_

#include <pdh.h> // Performance Data Helper

class CPUMonitor
{

public:

	CPUMonitor(){}
	CPUMonitor(const CPUMonitor&){}
	~CPUMonitor(){}

	void Init();
	void ShutDown();
	void Tick();
	int GetCPUPercentage();

private:

	bool m_canReadCPU;
	HQUERY m_queryHandle;
	HCOUNTER m_counterHandle;
	unsigned long m_lastSampleTime;
	long m_CPUUsage;
};

#endif