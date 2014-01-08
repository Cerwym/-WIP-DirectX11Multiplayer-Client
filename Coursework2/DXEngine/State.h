#ifndef STATE_H 
#define STATE_H

#include "Defines.h"
#include "Utilities\FPSMonitor.h"
#include "Utilities\CPUMonitor.h"
#include "Utilities\HPRTimer.h"
#include "D3DSys.h"
#include "D3DAudio.h"
#include "D3DInput.h"

class State
{
public:
	State( unsigned long id = 0 );
	~State();

	void State::Load( D3DSys* d3d, D3DAudio* audio, D3DInput* input )
	{
		baseLoad( d3d, audio,  input );
		dLoad();
	}

	void State::Close()
	{
		baseClose();
		dClose();
	}

	void State::Update()
	{
		baseUpdate();
		dUpdate( m_StateTime / 1000 );
	}

	void State::Render()
	{
		// Clear the screen
		m_Device->BeginScene( 0.0f, 0.0f, 0.0f, 1.0f );
		dRender(); // Render the derived state
		m_Device->EndScene();
	}
	
	unsigned long GetID(){ return m_ID; }

protected:

	// Calling the derived state's load is required, and MUST be overridden
	virtual void dLoad() = 0;
	// Calling the derived state's close is required, and MUST be overridden
	virtual void dClose() = 0;
	// Calling the derived state's update is required, and MUST be overridden
	virtual void dUpdate( float dt ) = 0;
	// Calling the derived state's render is required and MUST be overridden
	virtual void dRender() = 0;

	// Return the amount of time (MS) that have elapsed since the last call to Update
	float GetLastDelta() { return m_StateTime; }
	// Return the current percentage of the CPU used by the application.
	int GetCPU() { return m_CpuMonitor->GetCPUPercentage(); }
	// Return the current frame rate
	int GetFPS() { return m_FPSMonitor->GetFPS(); }

	D3DSys* D3D() { return m_Device; }
	D3DAudio* Audio() { return m_Audio; }
	D3DInput* Input() { return m_Input; }

private:

	void baseLoad( D3DSys* d3d, D3DAudio* audio, D3DInput* input );
	void baseClose();
	void baseUpdate();

	D3DSys* m_Device;
	D3DAudio* m_Audio; 
	D3DInput* m_Input;

	FPSMonitor* m_FPSMonitor;
	CPUMonitor* m_CpuMonitor;
	HPRTimer* m_HPRTimer;

	float m_StateTime;

private:
	unsigned long m_ID;
};

#endif