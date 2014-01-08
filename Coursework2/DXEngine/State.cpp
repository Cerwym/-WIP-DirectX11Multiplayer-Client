#include "State.h"

State::State( unsigned long id /* = 0 */ )
{
	m_ID = id;

	m_CpuMonitor = 0;
	m_CpuMonitor = new CPUMonitor;
	m_FPSMonitor = 0;
	m_FPSMonitor = new FPSMonitor;
	m_HPRTimer = 0;
	m_HPRTimer = new HPRTimer;
	m_RenderToTexture = 0;
	m_RenderToTexture = new D3DRenderToTexture;
}

State::~State()
{
	// Give the state access to the DirectX systems
	S_DELETE(m_CpuMonitor);
	S_DELETE(m_FPSMonitor);
	S_DELETE(m_HPRTimer);
	S_DELETE(m_RenderToTexture);
}

void State::baseLoad( EngineDefinition* eDef, D3DSys* d3d, D3DAudio* audio, D3DInput* input )
{
	m_Device = d3d;
	m_Audio = audio;
	m_Input = input;
	m_Definition = eDef;

	m_CpuMonitor->Init();
	m_FPSMonitor->Init();
	if (!m_HPRTimer->Init())
	{
		PostQuitMessage( 0 );
	}
	m_RenderToTexture->Initialize( m_Device->GetDevice(), eDef->scrWidth, eDef->scrHeight );
}

void State::baseClose()
{
	m_Audio->Cleanup();
}

void State::baseUpdate()
{
	m_HPRTimer->Tick();
	m_CpuMonitor->Tick();
	m_FPSMonitor->Tick();

	m_StateTime = m_HPRTimer->GetTime();
}