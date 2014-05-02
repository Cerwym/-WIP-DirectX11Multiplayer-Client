#ifndef HPRTIMER_H_
#define HPRTIMER_H_

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <stdlib.h>
#include <tchar.h>
#include <Mmsystem.h> //timeGetTime
#include <cmath>

const ULONG MAX_SAMPLE_COUNT = 50;
class HPRTimer
{
public:

	HPRTimer();
	HPRTimer(const HPRTimer&){}
	~HPRTimer(){}

	void Tick( float fLockFPS = 0.0f );
	float GetTime()const {return m_TimeElapsed;}
	unsigned long GetFrameRate( LPTSTR lpszString = NULL ) const;

private:

	// flag to holder whether the hardware performance counter is available on this system
	bool m_PerfHardware;
	// Scaling value to convert the queried value into seconds
	float m_TimeScale;
	// stores the current time in seconds that has elapsed since the previous Tick()
	float m_TimeElapsed;
	// Raw return from QPC function
	INT64 m_CurrentTime;
	INT64 m_LastTime;
	INT64 m_Frequency;
	// stores the elapsed values, MAX_SAMPLE_COUNT can be lowered and increased for averaging out frame values.
	float m_FrameTime[MAX_SAMPLE_COUNT];
	// Stores the amount of time samples stored in the array, this will be reduced over time
	unsigned long m_SampleCount;
	// The framerate calculated by the Tick() function
	unsigned long m_FrameRate;
	// stores the amount of times the Tick() function has been called in the space of 1 second
	unsigned long m_FPSFrameCount;
	float m_FPSTimeElapsed;
};

#endif