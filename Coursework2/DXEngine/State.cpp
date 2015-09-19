#include "State.h"

State::State( unsigned long id /* = 0 */, float locktoFPS /* = 0.0f */ )
{
	m_MaxFramerate = locktoFPS;
	m_ID = id;
	m_Device = 0;
	m_Audio = 0;
	m_Input = 0;
	m_Definition = 0;
	m_CpuMonitor = 0;
	m_HPRTimer = 0;
	m_RenderToTexture = 0;
	m_MouseBitmap = 0;
	FailedToLoadLast = false;
}

State::~State()
{
	// Give the state access to the DirectX systems
	S_DELETE(m_CpuMonitor);
	S_DELETE(m_HPRTimer);
	S_DELETE(m_RenderToTexture);
	S_DELETE( m_MouseBitmap );
}

bool State::baseLoad( EngineDefinition* eDef, D3DSys* d3d, D3DAudio* audio, D3DInput* input)
{
	m_Device = d3d;
	m_Audio = audio;
	m_Input = input;
	m_Definition = eDef;

	m_CpuMonitor = new CPUMonitor;
	m_CpuMonitor->Init();

	m_HPRTimer = new HPRTimer;
	m_RenderToTexture = new D3DRenderToTexture;
	if (!m_RenderToTexture->Initialize( m_Device->GetDevice(), eDef->scrWidth, eDef->scrHeight ))
		return false;

	m_MouseBitmap = new D3DBitmap;
	if ( !m_MouseBitmap->Init( m_Device->GetDevice(), eDef->scrWidth, eDef->scrHeight, L"Data/Textures/mouseCursor.dds", 32, 28 ) )
		return false;

	return true;
}

void State::baseClose()
{
	m_Audio->Cleanup();
	m_Device = 0;
	m_Audio = 0;
	m_Input = 0;
	m_MouseBitmap = 0;
}

void DebugOutput(WCHAR* szFormat, ...)
{
	WCHAR szBuff[1024];
	va_list arg;
	va_start(arg, szFormat);
	_vsnwprintf(szBuff, sizeof(szBuff), szFormat, arg);
	va_end(arg);

	OutputDebugString(szBuff);
}


void State::baseUpdate()
{
	// Lock to the state's specified maximum framerate
	m_HPRTimer->Tick( m_MaxFramerate );
	m_CpuMonitor->Tick();
	//DebugOutput( L"FrameRate %lu\n", m_HPRTimer->GetFrameRate());

	m_MouseBitmap->SetPosition(m_Input->GetPosX(), m_Input->GetPosY());

	m_StateTime = m_HPRTimer->GetTime();
}