#include "CPUMonitor.h"
#include <iostream>

void CPUMonitor::Init()
{
	PDH_STATUS status;

	// Initialize the flag indicating whether this object can read the system cpu usage or not.
	m_canReadCPU = true;

	// Create a query object to poll cpu usage.
	status = PdhOpenQuery(NULL, 0, &m_queryHandle);
	if(status != ERROR_SUCCESS)
		m_canReadCPU = false;

	// Set query object to poll all cpus in the system.
	status = PdhAddCounter(m_queryHandle, TEXT("\\Processor(_Total)\\% processor time"), 0, &m_counterHandle);
	if(status != ERROR_SUCCESS)
		m_canReadCPU = false;

	m_lastSampleTime = GetTickCount(); 

	m_CPUUsage = 0;


	return;
}

void CPUMonitor::Tick()
{
	PDH_FMT_COUNTERVALUE value; 

	if(m_canReadCPU)
	{
		// Only sample once a second to reduce the amount of queries
		if((m_lastSampleTime + 1000) < GetTickCount())
		{
			m_lastSampleTime = GetTickCount(); 
			PdhCollectQueryData(m_queryHandle);
			PdhGetFormattedCounterValue(m_counterHandle, PDH_FMT_LONG, NULL, &value);
			m_CPUUsage = value.longValue;
		}
	}
	return;
}

int CPUMonitor::GetCPUPercentage()
{
	int usage;

	if(m_canReadCPU)
		usage = (int)m_CPUUsage;
	else
		usage = 0;

	return usage;
}
