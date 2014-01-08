#include "MainMenuState.h"

MainMenuState::MainMenuState() : State( MAINMENU_STATE )
{
	m_Camera = 0;
	m_MarbleCube = 0;
	m_MetalCube = 0;
	m_StoneCube = 0;
	m_TextBatch = 0;
	m_Light = 0;
	g_Engine->DebugOutput(L"Main Menu Initialized\n");
	m_MouseLock = true;
}

void MainMenuState::dLoad()
{
	g_Engine->DebugOutput(L"Main Menu Loading...\n");
	m_OrthoMatrix = D3D()->GetOrthoMatrix();

	// Might want to check to see if the objects already exists as loading a new state will change what the object points to
	m_MarbleCube = new D3DModel(D3DXVECTOR3(-3.5f, 0, 0));
	m_MarbleCube->Init( D3D()->GetDevice(), "Data/Models/cube.txt", L"Data/Textures/marble.dds" );

	m_MetalCube = new D3DModel();
	m_MetalCube->Init( D3D()->GetDevice(), "Data/Models/cube.txt", L"Data/Textures/metal.dds" );
	
	m_Camera = new D3DCamera( 0.25f*3.14159265358979323f, (float) g_Engine->GetScreenWidth() / g_Engine->GetScreenHeight(), 0.1f, 1000.0f );
	m_Camera->SetPosition(D3DXVECTOR3(0.0f, 0.0f, -10.0f));
	
	// RebuildView must be called after instantiation so that the text will be drawn correctly.
	m_Camera->RebuildView();

	m_Light = new D3DLight;
	// Set to white and point down the positive x
	m_Light->SetAmbientColour(0.15f, 0.15f, 0.15f, 1.0f); // Set the intensity of the ambient light to be 15% of colour white
	m_Light->SetDiffuseColour(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(0.0f, 0.0f, 1.0f);
	m_Light->SetSpecularColour(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetSpecularPower(64.0f);

	m_TextBatch = new D3DText;
	m_TextBatch->Init( D3D()->GetDevice(), D3D()->GetDeviceContext(), g_Engine->GetWindow(), g_Engine->GetScreenWidth(), g_Engine->GetScreenHeight(), m_Camera->GetViewMatrix() );

	g_Engine->DebugOutput(L"Main Menu Loaded\n");
}

void MainMenuState::dClose()
{
	S_DELETE( m_Camera );
	S_DELETE( m_MetalCube );
	S_DELETE( m_StoneCube );
	S_DELETE( m_MarbleCube );
	S_DELETE( m_TextBatch );
	g_Engine->DebugOutput(L"Main Menu Closed\n");
}

void MainMenuState::dUpdate( float dt )
{

	if (Input()->isKeyPressed( DIK_ESCAPE) )
	{
		MessageBox( g_Engine->GetWindow(), L"Quitting Application", L"Quitting...", MB_ICONINFORMATION );
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

// 	if ( Input()->isKeyPressed( DIK_L, false ) )
// 		m_MouseLock = !m_MouseLock;

	if ( Input()->isKeyPressed( DIK_J, true ) )
		m_MetalCube->TranslateBy( -2.3f * dt, 0, 0 );

	if ( Input()->isKeyPressed( DIK_L, true ) )
		m_MetalCube->TranslateBy( 2.3f * dt, 0, 0 );

	if ( Input()->isKeyPressed( DIK_I, true ) )
		m_MetalCube->TranslateBy( 0, 0, 2.3f * dt );

	if ( Input()->isKeyPressed( DIK_K, true ) )
		m_MetalCube->TranslateBy( 0, 0, -2.3f * dt );

	if ( Input()->GetDeltaY() != 0 )
		m_Camera->Pitch( ( Input()->GetDeltaY() / g_Engine->GetMouseSensitivityY() ) * 0.0087266f );
	
	if ( Input()->GetDeltaX() != 0 )
		m_Camera->Roll( ( Input()->GetDeltaX() / g_Engine->GetMouseSensitivityX() ) * 0.0087266f );

	if (! m_TextBatch->SetMousePosition( Input()->GetPosX(), Input()->GetPosY(), D3D()->GetDeviceContext() ) )
	{
		MessageBox( g_Engine->GetWindow(), L"Error updating shitty text", L"Error", MB_OK );
		PostQuitMessage( 0 );
	}

	m_TextBatch->SetCPU(GetCPU(), D3D()->GetDeviceContext());
	m_TextBatch->SetFPS(GetFPS(), D3D()->GetDeviceContext());

	m_MarbleCube->RotateBy( 1 * dt, 0, 0 );
	m_MetalCube->RotateBy( 0, 1 * dt, 0 );

	if (m_MouseLock)
		g_Engine->LockMouseToCentre();

	if( Input()->isKeyPressed( DIK_C ) )
		g_Engine->ChangeState( GAME_STATE );
}

void MainMenuState::dRender()
{
	m_Camera->RebuildView();

	m_ViewMatrix = m_Camera->GetViewMatrix();
	m_ProjectionMatrix = m_Camera->GetProjMatrix();

	m_MarbleCube->Render( D3D()->GetDeviceContext(), g_Engine->ShaderManager(), m_ViewMatrix, m_ProjectionMatrix, m_Camera->GetPosition(), m_Light );

	// Passing in a light object will make the cube draw against the light shader
	m_MetalCube->Render( D3D()->GetDeviceContext(), g_Engine->ShaderManager(), m_ViewMatrix, m_ProjectionMatrix, m_Camera->GetPosition(), m_Light );
	
	// Being 2D rendering
	D3D()->TurnZBufferOff();
	D3D()->TurnOnAlphaBlending();

	if (!m_TextBatch->Render( D3D()->GetDeviceContext(), m_OrthoMatrix ))
	{
		MessageBox( g_Engine->GetWindow(), L"Error drawing shitty text", L"Error", MB_OK );
		PostQuitMessage( 0 );
	}

	D3D()->TurnOffAlphaBlending();
	D3D()->TurnZBufferOn();

}
