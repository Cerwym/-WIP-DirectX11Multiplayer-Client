#include "GameState.h"

GameState::GameState() : State( GAME_STATE )
{
	m_Camera = 0;
	m_Cube = 0;
	g_Engine->DebugOutput(L"Main Menu Initialized\n");
}

void GameState::dLoad()
{
	g_Engine->DebugOutput(L"Main Menu Loading...\n");
	m_Cube = new D3DModel(D3DXVECTOR3(0, 0, 0));
	m_Cube->Init( D3D()->GetDevice(), "Data/Models/cube.txt", L"Data/Textures/marble.dds" );

	m_Camera = new D3DCamera( 0.25f*3.14159265358979323f, (float) g_Engine->GetScreenWidth() / g_Engine->GetScreenHeight(), 0.1f, 1000.0f );
	m_Camera->SetPosition(D3DXVECTOR3(0.0f, 0.0f, -10.0f));
	g_Engine->DebugOutput(L"Main Menu Loaded\n");
}

void GameState::dClose()
{
	S_DELETE( m_Camera );
	S_DELETE( m_Cube );

	// Call audio cleanup

	g_Engine->DebugOutput(L"Main Menu Closed\n");
}

void GameState::dUpdate( float dt )
{
	if ( Input()->isKeyPressed( DIK_ESCAPE) )
	{
		MessageBox( g_Engine->GetWindow(), L"Quitting Application", L"Quitting...", MB_ICONINFORMATION) ;
		PostQuitMessage( 0 );
	}

	if( Input()->isKeyPressed( DIK_P ) )
		Audio()->PlayFile( "Data/Audio/sound01.wav" );

	// Camera movement
	if ( Input()->isKeyPressed( DIK_A , true ) )
		m_Camera->Move_X(-2.3f * dt);

	if ( Input()->isKeyPressed( DIK_D, true ) )
		m_Camera->Move_X(2.3f * dt);

	if ( Input()->isKeyPressed( DIK_W , true ) )
		m_Camera->Move_Z(2.3f * dt);

	if ( Input()->isKeyPressed( DIK_S, true ) )
		m_Camera->Move_Z(-2.3f * dt);

	if ( Input()->GetDeltaY() != 0 )
		m_Camera->Pitch( ( Input()->GetDeltaY() / g_Engine->GetMouseSensitivityY() ) * 0.0087266f );

	if ( Input()->GetDeltaX() != 0 )
		m_Camera->Roll( ( Input()->GetDeltaX() / g_Engine->GetMouseSensitivityX() ) * 0.0087266f );

	if( Input()->isKeyPressed( DIK_C ) )
		g_Engine->ChangeState( MAINMENU_STATE );
}

void GameState::dRender()
{
	D3D()->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	m_Camera->RebuildView();

	m_Cube->Render( D3D()->GetDeviceContext(), g_Engine->ShaderManager(), m_Camera->GetViewMatrix(), m_Camera->GetProjMatrix() );
}