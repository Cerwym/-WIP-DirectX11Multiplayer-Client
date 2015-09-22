#include "MainMenuState.h"

MainMenuState::MainMenuState() : State( MAINMENU_STATE )
{
	m_Camera = 0;
	m_MarbleCube = 0;
	m_MetalCube = 0;
	m_StoneCube = 0;
	m_TextBatch = 0;
	m_Light = 0;
	m_DebugView = 0;
	m_DebugView2 = 0;
	m_DebugView3 = 0;
	m_SkyBox = 0;
	m_RainEmitter = 0;

	g_Engine->DebugOutput(L"Main Menu Initialized\n");
	m_MouseLock = true;
}

bool MainMenuState::dLoad()
{
	/*
	g_Engine->DebugOutput(L"Main Menu Loading...\n");
	m_OrthoMatrix = D3D()->GetOrthoMatrix();

	// Might want to check to see if the objects already exists as loading a new state will change what the object points to
	m_MarbleCube = new D3DModel(XMFLOAT3(-3.5f, 0, 0));
	if(!m_MarbleCube->Init( D3D()->GetDevice(), D3D()->GetDeviceContext(), "Data/Models/cube.txt", "Data/Textures/marble.dds"))
		return false;

	m_MetalCube = new D3DModel();
	if (!m_MetalCube->Init(D3D()->GetDevice(), D3D()->GetDeviceContext(), "Data/Models/cube.txt", "Data/Textures/metal.dds"))
		return false;

	m_StoneCube = new D3DModel(XMFLOAT3(3.5f, 0, 0));
	if (!m_StoneCube->Init(D3D()->GetDevice(), D3D()->GetDeviceContext(), "Data/Models/cube.txt", "Data/Textures/stone.dds", "Data/Models/cube_normal.dds"))
		return false;

	m_SkyBox = new D3DSkyBox;
	if(!m_SkyBox->Init( D3D()->GetDevice(), 10, 10, L"Data/Textures/skymap.dds", g_Engine->GetWindow()))
		return false;
	
	m_Camera = new D3DCamera( 0.25f*3.14159265358979323f, (float) Options()->scrWidth / Options()->scrHeight, 0.1f, 1000.0f );
	m_Camera->SetPosition(XMFLOAT3(0.0f, 0.0f, -10.0f));
	
	// RebuildView must be called after instantiation so that the text will be drawn correctly.
	m_Camera->RebuildView();
	m_baseViewMatrix = m_Camera->GetViewMatrix();

	m_Light = new D3DLight;
	// Set to white and point down the positive x
	m_Light->SetAmbientColour(0.15f, 0.15f, 0.15f, 1.0f); // Set the intensity of the ambient light to be 15% of colour white
	m_Light->SetDiffuseColour(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(0.0f, 0.0f, 1.0f);
	m_Light->SetSpecularColour(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetSpecularPower(64.0f);

	m_TextBatch = new D3DText;
	if (!m_TextBatch->Init( D3D()->GetDevice(), D3D()->GetDeviceContext(), g_Engine->GetWindow(), g_Engine->GetScreenWidth(), g_Engine->GetScreenHeight(), m_baseViewMatrix ))
		return false;

	m_DebugView = new D3DBitmap;
	if (!m_DebugView->Init( D3D()->GetDevice(), D3D()->GetDeviceContext(), Options()->scrWidth, Options()->scrHeight, NULL, 100, 100 ))
		return false;

	m_DebugView->SetPosition(50, 50);

	m_RainEmitter = new	D3DParticleEmitter;
	if (!m_RainEmitter->Initialize( D3D()->GetDevice(), D3D()->GetDeviceContext(), "Data/Textures/raindrop.dds"))
		return false;
 */
	g_Engine->DebugOutput(L"Main Menu Loaded\n"); 
	return true;
}

void MainMenuState::dClose()
{
	S_DELETE( m_Camera );
	S_DELETE( m_MetalCube );
	S_DELETE( m_StoneCube );
	S_DELETE( m_MarbleCube );
	S_DELETE( m_TextBatch );
	S_DELETE( m_DebugView );
	S_DELETE( m_DebugView2 );
	S_DELETE( m_DebugView3 );
	S_DELETE( m_RainEmitter );
	S_DELETE( m_SkyBox );

	g_Engine->DebugOutput(L"Main Menu Closed\n");
}

void MainMenuState::dUpdate( float dt )
{
	g_Engine->DebugOutput(L"Updating MainMenuState %f\n", dt);
	
	if (Input()->isKeyPressed( DIK_ESCAPE) )
	{
		MessageBox( g_Engine->GetWindow(), L"Quitting Application", L"Quitting...", MB_ICONINFORMATION );
		PostQuitMessage( 0 );
	}
	
	if( Input()->isKeyPressed( DIK_P ) )
		Audio()->PlayFile( "Data/Audio/sound01.wav" );
	
	/*
	// Camera movement
	if ( Input()->isKeyPressed( DIK_A , true ) )
		m_Camera->Move_X(-2.3f * dt);
	if ( Input()->isKeyPressed( DIK_D, true ) )
		m_Camera->Move_X(2.3f * dt);

	if ( Input()->isKeyPressed( DIK_W , true ) )
		m_Camera->Move_Z(2.3f * dt);
	
	if ( Input()->isKeyPressed( DIK_S, true ) )
		m_Camera->Move_Z(-2.3f * dt);

 	if ( Input()->isKeyPressed( DIK_L, false ) )
 		m_MouseLock = !m_MouseLock;

	if ( Input()->GetDeltaY() != 0 && m_MouseLock )
		m_Camera->Pitch( ( Input()->GetDeltaY() / g_Engine->GetMouseSensitivityY() ) * 0.0087266f );
	
	if ( Input()->GetDeltaX() != 0 && m_MouseLock)
		m_Camera->Roll( ( Input()->GetDeltaX() / g_Engine->GetMouseSensitivityX() ) * 0.0087266f );

	*/



	/*
	if (! m_TextBatch->SetMousePosition( Input()->GetPosX(), Input()->GetPosY(), D3D()->GetDeviceContext() ) )
	{
		MessageBox( g_Engine->GetWindow(), L"Error updating shitty text", L"Error", MB_OK );
		PostQuitMessage( 0 );
	}

	m_TextBatch->SetCPU(GetCPU(), D3D()->GetDeviceContext());
	m_TextBatch->SetFPS(GetFPS(), D3D()->GetDeviceContext());

	*/


	/*
	m_MarbleCube->RotateBy( 1 * dt, 0, 0 );
	m_MetalCube->RotateBy( 0, 1 * dt, 0 );
	m_StoneCube->RotateBy( 1 * dt , 0, 1 * dt );

	m_RainEmitter->Frame(dt *5000, D3D()->GetDeviceContext(), m_Camera);
	m_RainEmitter->TranslateTo( m_Camera->GetPosition().x, m_Camera->GetPosition().y + 3, m_Camera->GetPosition().z + 3 );
	m_RainEmitter->SetRotation( Input()->GetDeltaX() / g_Engine->GetMouseSensitivityX()  * 0.0087266f, Input()->GetDeltaY() / g_Engine->GetMouseSensitivityY() * 0.0087266f, 0 );

	if (WindowIsActive() && m_MouseLock)
		g_Engine->LockMouseToCentre();
	
	if( Input()->isKeyPressed( DIK_O ) )
	{
		g_Engine->ChangeState( OPTIONS_STATE );
		return;
	}

	if( Input()->isKeyPressed( DIK_C ) )
	{
		g_Engine->ChangeState( GAME_STATE );
		if (FailedToLoadLast)
		{
			g_Engine->DebugOutput(L"There was an error loading this state, ignoring request...\n");
		}
		return;
	}

	*/
}

void MainMenuState::dRender()
{

	/*
	RenderTexture()->SetRenderTarget( D3D()->GetDeviceContext(), D3D()->GetDepthStencilView() );
	RenderTexture()->ClearRenderTarget( D3D()->GetDeviceContext(), D3D()->GetDepthStencilView(), 0.0f, 0.0f, 0.0f, 1.0f);
	// Render the scene (CURRENTLY WITH ALL SHADERS on the front buffer)
	
	D3D()->TurnOnWireFrame();
	Render3D( false, false );
	D3D()->TurnOffWireFrame();

	m_DebugView->SetTexture( RenderTexture()->GetShaderResourceView() );
	// Continue rendering as normal on the back buffer
	
	D3D()->SetBackBufferRenderTarget();
	D3D()->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);
	
	Render3D();
	Render2D();

	*/
}

void MainMenuState::Render3D( bool renderParticles, bool renderSkybox )
{
	m_Camera->RebuildView();
	D3D()->TurnOnWireFrame();
	if ( renderSkybox )
	{
		D3D()->TurnZBufferOff();
		D3D()->TurnOffCulling();
		m_SkyBox->Draw( D3D()->GetDeviceContext(), m_Camera );
		D3D()->TurnOnCulling();
		D3D()->TurnZBufferOn();
	}

	m_MarbleCube->Render( D3D()->GetDeviceContext(), g_Engine->ShaderManager(), m_Camera, NULL );

	// Passing in a light object will make the cube draw against the light shader
	m_MetalCube->Render( D3D()->GetDeviceContext(), g_Engine->ShaderManager(), m_Camera, m_Light ) ;

	m_StoneCube->Render( D3D()->GetDeviceContext(), g_Engine->ShaderManager(), m_Camera, m_Light );

	// Put the particle system vertex and index buffers on the graphics pipeline to prepare them for drawing.

	if (renderParticles)
	{
		D3D()->TurnOnAlphaBlending();
		m_RainEmitter->NO_GSBillboard( m_Camera );
		m_RainEmitter->Render(D3D()->GetDeviceContext());

		// Render the model using the texture shader.
		g_Engine->ShaderManager()->RenderTextureShader(D3D()->GetDeviceContext(), m_RainEmitter->GetIndexCount(), m_RainEmitter->GetWorld(), m_Camera->GetViewMatrix(), m_Camera->GetProjMatrix(), 
			m_RainEmitter->GetTexture());
		D3D()->TurnOffAlphaBlending();
	}

	D3D()->TurnOffWireFrame();
}

void MainMenuState::Render2D()
{
	// Being 2D rendering
	D3D()->TurnZBufferOff();
	D3D()->TurnOnAlphaBlending();

	if (!m_TextBatch->Render( D3D()->GetDeviceContext(), m_OrthoMatrix ))
	{
		MessageBox( g_Engine->GetWindow(), L"Error drawing shitty text", L"Error", MB_OK );
		PostQuitMessage( 0 );
	}
	
	D3D()->TurnOffAlphaBlending();
	// The debug view should always be on top and thus, not blend with the scene
	m_DebugView->Render( D3D()->GetDeviceContext(), g_Engine->ShaderManager(), m_baseViewMatrix, m_OrthoMatrix ) ;
	D3D()->TurnZBufferOn();
}