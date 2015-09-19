#include "OptionsState.h"

OptionsState::OptionsState() : State( OPTIONS_STATE )
{
	m_Camera = 0;
	m_BackgroundTexture = 0;
	m_Button = 0;
}

bool OptionsState::dLoad()
{
	g_Engine->DebugOutput( L"OptionsState Loading..." );
	m_Camera = new D3DCamera( 0.25f*3.14159265358979323f, (float) g_Engine->GetScreenWidth() / g_Engine->GetScreenHeight(), 0.1f, 1000.0f );
	m_Camera->SetPosition(XMFLOAT3(0.0f, 0.0f, -10.0f));
	m_Camera->RebuildView();
	m_baseViewMatrix = m_Camera->GetViewMatrix();

	m_BackgroundTexture = new D3DBitmap;
	if (!m_BackgroundTexture->Init( D3D()->GetDevice(), D3D()->GetDeviceContext(), Options()->scrWidth, Options()->scrHeight, "Data/Textures/options_background.tga", Options()->scrWidth, Options()->scrHeight ))
	{
		MessageBox( 0, L"Failed to load background texture", L"Data/Textures/options_background.dds", MB_OK );
		return 0;
	}
	m_BackgroundTexture->SetPosition(0,0);

	m_Button = new D3DBitmap;
	if (!m_Button->Init( D3D()->GetDevice(), D3D()->GetDeviceContext(), Options()->scrWidth, Options()->scrWidth, "Data/Textures/UI/button.tga", 412,90))
	{
		MessageBox( 0, L"Failed to load button texture", L"Data/Textures/UI/button.tga", MB_OK);
		return 0;
	}
	m_Button->SetPosition( 20, 180);
	
	g_Engine->DebugOutput( L"OptionsState Loaded" );
	return true;
}

void OptionsState::dClose()
{
	S_DELETE( m_Camera );
	S_DELETE( m_BackgroundTexture );
	S_DELETE( m_Button );
}

void OptionsState::dUpdate( float dt )
{
	if ( Input()->isKeyPressed( DIK_C) )
	{
		g_Engine->ChangeState( MAINMENU_STATE );
		return;
	}

	if ( Input()->isKeyPressed( DIK_ESCAPE) )
	{
		MessageBox(0, L"Application Quitting", L"Quitting...", MB_OK);
		PostQuitMessage( 0 );
	}
}

void OptionsState::dRender()
{
	m_Camera->RebuildView();
	D3D()->TurnZBufferOff();
	
	D3D()->TurnOnAlphaBlending();
	
	m_Button->Render( D3D()->GetDeviceContext(), g_Engine->ShaderManager(), m_baseViewMatrix, D3D()->GetOrthoMatrix() );
	m_BackgroundTexture->Render( D3D()->GetDeviceContext(), g_Engine->ShaderManager(), m_baseViewMatrix, D3D()->GetOrthoMatrix() );

	D3D()->TurnOffAlphaBlending();
	TEMPMOUSE()->Render( D3D()->GetDeviceContext(), g_Engine->ShaderManager(), m_baseViewMatrix, D3D()->GetOrthoMatrix() );
	D3D()->TurnZBufferOn();
}