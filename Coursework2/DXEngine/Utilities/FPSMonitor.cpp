#include "FPSMonitor.h"

void FPSMonitor::Init()
{
	m_FPS = 0;
	m_count = 0;
	m_startTime = timeGetTime();
	return;
}

void FPSMonitor::Tick()
{
	m_count++;

	if (timeGetTime() >= (m_startTime + 1000))
	{
		m_FPS = m_count;
		m_count = 0;
		m_startTime = timeGetTime();
	}
}