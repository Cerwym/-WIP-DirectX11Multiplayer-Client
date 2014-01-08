#include "HPRTimer.h"

bool HPRTimer::Init()
{
	// Check to see if this system supports high performance timers.
	QueryPerformanceFrequency((LARGE_INTEGER*)&m_Frequency);
	if(m_Frequency == 0)
		return false;

	// Find out how many times the frequency counter ticks every millisecond.
	m_TicksPerMs = (float)(m_Frequency / 1000);

	QueryPerformanceCounter((LARGE_INTEGER*)&m_StartTime);

	return true;
}

void HPRTimer::Tick()
{
	INT64 currentTime;
	float timeDifference;
	
	QueryPerformanceCounter((LARGE_INTEGER*)& currentTime);
	
	timeDifference = (float)(currentTime - m_StartTime);
	m_FrameTime = timeDifference / m_TicksPerMs;
	m_StartTime = currentTime;

	return;
}