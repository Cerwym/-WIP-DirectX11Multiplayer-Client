#ifndef _HPRTIMER_H_
#define _HPRTIMER_H_

#include <windows.h>

class HPRTimer
{

public:

	HPRTimer(){}
	HPRTimer(const HPRTimer&){}
	~HPRTimer(){}

	bool Init();
	void Tick();

	float GetTime(){return m_FrameTime;}

private:

	INT64 m_Frequency;
	float m_TicksPerMs;
	INT64 m_StartTime;
	float m_FrameTime;
};

#endif