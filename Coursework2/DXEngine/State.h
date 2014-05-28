#pragma once

#include <process.h> // For threading
#include "Defines.h"
#include "EngineDefinition.h"
#include "Objects\D3DBitmap.h"
#include "Utilities\CPUMonitor.h"
#include "Utilities\HPRTimer.h"
#include "Utilities\D3DRenderToTexture.h"
#include "D3DSys.h"
#include "D3DAudio.h"
#include "D3DInput.h"

class State
{
public:
	State( unsigned long id = 0, float locktoFPS = 60.0f );
	~State();

	bool State::Load( EngineDefinition* eDef, D3DSys* d3d, D3DAudio* audio, D3DInput* input )
	{
		// Start a thread to begin
		if (!baseLoad( eDef, d3d, audio,  input ))
			return false;

		if (!dLoad())
		{
			MessageBox(0, L"State Failed to load correctly, check Init()", L"Error", MB_OK);
			return false;
		}
		
		isLoaded = true;
		return true;
	}

	void State::Close()
	{
		baseClose();
		dClose();
	}

	void State::Update()
	{
		baseUpdate();
		dUpdate( m_StateTime );
	}

	void State::Render()
	{
		// Clear the screen
		m_Device->BeginScene( 0.0f, 0.0f, 0.0f, 1.0f );
		dRender(); // Render the derived state
		m_Device->EndScene();
	}

	bool State::IsLoaded(){ return isLoaded;}

	void SetFailed(){ FailedToLoadLast = true;}
	
	unsigned long GetID(){ return m_ID; }

protected:

	// Calling the derived state's load is required, and MUST be overridden
	virtual bool dLoad() = 0;
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
	int GetFPS() { return (int)m_HPRTimer->GetFrameRate(); }
	bool isLoaded;

	D3DSys* D3D() { return m_Device; }
	D3DAudio* Audio() { return m_Audio; }
	D3DInput* Input() { return m_Input; }
	D3DRenderToTexture* RenderTexture() { return m_RenderToTexture; }
	EngineDefinition* Options() { return m_Definition; }
	// Temporary, clean this up
	D3DBitmap* TEMPMOUSE() {return m_MouseBitmap; }
	
	bool FailedToLoadLast;

private:

	bool baseLoad( EngineDefinition* eDef, D3DSys* d3d, D3DAudio* audio, D3DInput* input);
	void baseClose();
	void baseUpdate();

	D3DSys* m_Device;
	D3DAudio* m_Audio; 
	D3DInput* m_Input;
	D3DRenderToTexture* m_RenderToTexture;
	EngineDefinition* m_Definition;

	// Bitmap used to store the mouse cursor's texture
	D3DBitmap* m_MouseBitmap;

	CPUMonitor* m_CpuMonitor;
	HPRTimer* m_HPRTimer;

	float m_StateTime;
	float m_MaxFramerate;

private:
	unsigned long m_ID;
};