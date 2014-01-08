#ifndef _FPSMONITOR_H_
#define _FPSMONITOR_H_

#include <windows.h>
#include <mmsystem.h>

class FPSMonitor
{

public:

	FPSMonitor(){}
	FPSMonitor(const FPSMonitor&){}
	~FPSMonitor(){}

	// There may be cases where the FPS needs to be reset, rather than the object deleted
	void Init();
	void Tick();
	int GetFPS(){return m_FPS;}

private:

	int m_FPS, m_count;
	unsigned long m_startTime;
};

#endif