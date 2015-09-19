#include "LoadingState.h"

LoadingState::LoadingState() : State( LOADING_STATE )
{
	m_Camera = 0;
	m_LoadingBitmap = 0;
	m_Imp = 0;
	impMX = 0;
}

bool LoadingState::dLoad()
{
	g_Engine->DebugOutput(L"State->Loading...\n");

	m_Camera = new D3DCamera( 0.25f*3.14159265358979323f, (float) g_Engine->GetScreenWidth() / g_Engine->GetScreenHeight(), 0.1f, 1000.0f );
	m_Camera->SetPosition(XMFLOAT3(0.0f, 0.0f, -10.0f));
	m_Camera->RebuildView();
	m_baseViewMatrix = m_Camera->GetViewMatrix();

	m_LoadingBitmap = new D3DBitmap;
	if (!m_LoadingBitmap->Init( D3D()->GetDevice(), D3D()->GetDeviceContext(), Options()->scrWidth, Options()->scrHeight, "Data/Textures/loadingtexture.tga", Options()->scrWidth, Options()->scrHeight ))
	{
		MessageBox(0, L"Error Loading Texture", L"Error", MB_OK);
		return false;
	}

	m_Imp = new D3DBitmap;
	if (!m_Imp->Init(D3D()->GetDevice(), D3D()->GetDeviceContext(), Options()->scrWidth, Options()->scrHeight, "Data/Textures/imp.tga", 50, 50))
	{
		MessageBox(0, L"Error loading imp texture", L"Error", MB_OK);
		return false;
	}
	m_LoadingBitmap->SetPosition(0, 0);
	m_Imp->SetPosition(m_Imp->GetWidth(), Options()->scrHeight - m_Imp->GetHeight());

	return true;
}

void LoadingState::dClose()
{
	S_DELETE( m_Camera );
	S_DELETE( m_LoadingBitmap );

	g_Engine->DebugOutput(L"Loading State Closed\n");
}

void LoadingState::dUpdate( float dt )
{
	static int posXBy = -4;

	if (m_Imp->GetPosition().x <= 0 || m_Imp->GetPosition().x >= (Options()->scrWidth - m_Imp->GetWidth() ))
		posXBy =-posXBy;

	m_Imp->MoveBy(posXBy, 0);
}

void LoadingState::dRender()
{
	m_Camera->RebuildView();
	D3D()->TurnZBufferOff();
	D3D()->TurnOnAlphaBlending();

	m_Imp->Render(D3D()->GetDeviceContext(), g_Engine->ShaderManager(), m_baseViewMatrix, D3D()->GetOrthoMatrix());
	m_LoadingBitmap->Render(D3D()->GetDeviceContext(), g_Engine->ShaderManager(), m_baseViewMatrix, D3D()->GetOrthoMatrix());

	D3D()->TurnOffAlphaBlending();

	D3D()->TurnZBufferOn();

}