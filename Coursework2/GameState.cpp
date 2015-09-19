#include "GameState.h"

GameState::GameState() : State( GAME_STATE )
{
	m_BillBoard = 0;
	m_GameUI = 0;
	m_Light = 0;
	m_Floor = 0;
	m_ParticleSystem = 0;
	m_MouseLock = true;
	m_SkyBox = 0;
	m_DaveDude = 0;
	m_Network = 0;
	m_NetworkEntityList = 0;
	m_TypingMessage = false;
	m_Player = 0;
	g_Engine->DebugOutput(L"Game State Initialized\n");
}

bool GameState::dLoad()
{
	HWND appWindow = g_Engine->GetWindow();
	g_Engine->DebugOutput(L"Main Menu Loading...\n");

	m_Player = new Player;
	m_Player->Init( 0.25f*3.14159265358979323f, (float) g_Engine->GetScreenWidth() / g_Engine->GetScreenHeight(), 0.1f, 1000.0f );
	m_Player->SetPosition(0.0f, -2.0f, -10.0f);

	m_Network = new NWSystem;
	if (!m_Network->Init(0,0))
	{
		MessageBox(appWindow, L"Could not init the network object.", L"Error", MB_OK);
	}

	m_NetworkEntityList = new NWEntity[MAX_ENTITIES];
	if (!m_NetworkEntityList)
	{
		MessageBox(appWindow, L"Could not init the entity list array", L"Error", MB_OK );
		return false;
	}
	
	m_Floor = new D3DModel(XMFLOAT3(0, -5, 0));
	m_Floor->Init( D3D()->GetDevice(), "Data/Models/floor.txt", L"Data/Textures/floor.dds", L"Data/Textures/floor_nmap.dds");

	m_BillBoard = new D3DModel(XMFLOAT3(0, -4, 5));
	if (!m_BillBoard->Init( D3D()->GetDevice(), "Data/Models/square.txt", L"Data/Textures/imp.dds"))
	{
		MessageBox( appWindow, L"Failed to Load billboard model", L"Error", MB_OK);
		return false;
	}

	m_DaveDude = new D3DModel(XMFLOAT3(0,0,0));
	if (!m_DaveDude->Init( D3D()->GetDevice(), "Data/Models/dude.txt", L"Data/Textures/dude_texture.dds"))
	{
		MessageBox( appWindow, L"Failed to Load dave model correctly", L"Error", MB_OK);
		return false;
	}

	// Create the particle system object.
	
	m_ParticleSystem = new D3DParticleEmitter;
	// Initialize the particle system object.
	if(!m_ParticleSystem->Initialize(D3D()->GetDevice(), L"Data/Textures/raindrop.dds"))
	{
		MessageBox(appWindow, L"Could not initialize the particle shader object.", L"Error", MB_OK);
		return false;
	}

	m_SkyBox = new D3DSkyBox;
	if ( !m_SkyBox->Init( D3D()->GetDevice(), 10, 10, L"Data/Textures/skymap.dds", g_Engine->GetWindow()) )
	{
		MessageBox( appWindow, L"Error in creating the skybox", L"Error", MB_OK);
		return false;
	}

	m_Player->Camera()->RebuildView();

	m_GameUI = new D3DUI;
	if (!m_GameUI->Init(D3D(), Options()->scrWidth, Options()->scrHeight, 600, 140, m_Player->Camera()->GetViewMatrix(), 100, 455))
		return false;

	m_GameUI->SetPosition(100, 445);
	
	m_Light = new D3DLight;
	// Set to white and point down the positive x
	m_Light->SetAmbientColour(0.25f, 0.25f, 0.25f, 1.0f); // Set the intensity of the ambient light to be 25% of colour white
	m_Light->SetDiffuseColour(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(0.0f, -1.0f, 0.0f);
	m_Light->SetSpecularColour(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetSpecularPower(32.0f);

	// Set the UI and GameState pointers, doing this here as one of the improvements I wish to make is allowing the state to continue loading whilst waiting for a response from the server.
	if (m_Network)
	{
		m_Network->SetUIAndGS(this, m_GameUI);
		m_Network->SetPingWaitTime(3000); // Send a ping message every 3 seconds.
	}

	bool statusConnected = m_Network->ConnectToServer( Options()->server_addr, Options()->server_port, 2000 );
	if (!statusConnected)
	{
		MessageBox(NULL, L"Could not connect to the server, your game will have networking functionality!", L"Error", MB_OK);
	}
	else
	{
		if (m_Network)
		m_Network->RequestEntityList();
	}

	g_Engine->DebugOutput(L"Game State Loaded\n");
	return true;
}

void GameState::dClose()
{
	S_DELETE( m_BillBoard );
	S_DELETE( m_Floor );
	S_DELETE( m_SkyBox );
	S_DELETE( m_DaveDude );
	S_DELETE( m_Light );
	S_DELETE( m_ParticleSystem );
	S_DELETE( m_Player);

	// Tell the server that we are disconnecting.

	m_Network->SendDisconnectMessage();

	g_Engine->DebugOutput(L"Game State Closed\n");
}

void GameState::dUpdate( float dt )
{
	// Update the player first
	m_Player->Update(Input(), dt);
	XMFLOAT3 pos = m_Player->GetPosition();
	XMFLOAT3 rot = m_Player->GetRotation();
	XMFLOAT3 vel = m_Player->GetVelocity();
	XMFLOAT3 acc = m_Player->GetAcceleration();
	// Update network information


	// Set all the network entities message flag to false to see whether a message has been recived from the server on this frame.
	for (int i = 0; i < MAX_ENTITIES; i++)
	{
		if (m_NetworkEntityList[i].IsOnline())
		{
			m_NetworkEntityList[i].HasRecievedMessage(false);
		}
	}

	m_Network->Update();

	g_Engine->DebugOutput(L"Accelerating = (%f, %f, %f), Velocity = (%f, %f, %f)\n", acc.x, acc.y, acc.z, vel.x, vel.y, vel.z);

	m_Network->SendPositionUpdate(pos, rot, vel, acc);
	

	if ( WindowIsActive() && m_MouseLock ) // && window is active
		g_Engine->LockMouseToCentre();
	
	m_ParticleSystem->Frame(dt *2000, D3D()->GetDeviceContext(), m_Player->Camera());
	m_ParticleSystem->TranslateTo( pos.x, pos.y + 3, pos.z + 3 );
	
	m_DaveDude->RotateBy( 0, 1 * dt, 0 );

	for (int i = 0; i < MAX_ENTITIES; i++)
	{
		if (m_NetworkEntityList[i].IsOnline()){}
			m_NetworkEntityList[i].Update(dt);
	}

	UpdateInput( dt );
}

void GameState::UpdateInput( float dt )
{
	bool positionChanged = false;
	if (!m_TypingMessage)
	{

		if ( Input()->isKeyPressed( DIK_ESCAPE) )
		{
			MessageBox( g_Engine->GetWindow(), L"Quitting Application", L"Quitting...", MB_ICONINFORMATION) ;
			PostQuitMessage( 0 );
		}

		if( Input()->isKeyPressed( DIK_P ) )
			Audio()->PlayFile( "Data/Audio/sound01.wav" );

		// Camera movement
		/*
		if ( Input()->isKeyPressed( DIK_A , true ) )
		{
			m_Player->MoveX(-2.3f * dt);
			positionChanged = true;
		}

		if ( Input()->isKeyPressed( DIK_D, true ) )
		{
			m_Player->MoveX(2.3f * dt);
			positionChanged = true;
		}

		if ( Input()->isKeyPressed( DIK_W , true ) )
		{
			m_Player->MoveZ(2.3f * dt);
			positionChanged = true;
		}

		if ( Input()->isKeyPressed( DIK_S, true ) )
		{
			m_Player->MoveZ(-2.3f * dt);
			positionChanged = true;
		}
		*/

		if ( Input()->isKeyPressed( DIK_J, true ) )
			m_DaveDude->TranslateBy( -2.3 * dt, 0, 0);
		if ( Input()->isKeyPressed( DIK_L, true ) )
			m_DaveDude->TranslateBy( 2.3 * dt, 0, 0);
		if ( Input()->isKeyPressed( DIK_I, true ) )
			m_DaveDude->TranslateBy( 0 , 0, 2.3 * dt);
		if ( Input()->isKeyPressed( DIK_K, true ) )
			m_DaveDude->TranslateBy( 0 , 0, -2.3 * dt);

		if ( Input()->isKeyPressed( DIK_T, true ) )
			m_Floor->RotateBy(0, 0, -2.3 * dt);

		if ( Input()->isKeyPressed( DIK_G, true ) )
			m_Floor->RotateBy(0, 0, 2.3 * dt);

		if ( Input()->GetDeltaY() != 0 && m_MouseLock )
			m_Player->Camera()->Pitch( ( Input()->GetDeltaY() / g_Engine->GetMouseSensitivityY() ) * 0.0087266f );

		if ( Input()->GetDeltaX() != 0 && m_MouseLock)
			m_Player->Camera()->Roll( ( Input()->GetDeltaX() / g_Engine->GetMouseSensitivityX() ) * 0.0087266f );

		if( Input()->isKeyPressed( DIK_C ) )
		{
			g_Engine->ChangeState( MAINMENU_STATE );

			// I will want to change this to some form of timeout, but instead i'm just going to print out the indication of previous failure.
			if (FailedToLoadLast)
			{
				g_Engine->DebugOutput(L"There was an error loading this state, ignoring request...\n");
			}
		}
	}
	else
	{
		// Capture key presses and send to the user interface
		int key;
		Input()->UpdateKeys();
		Input()->GetNewKeyPress(key);
		m_GameUI->KeyPressUpdate(key);
	}

	if (Input()->isKeyPressed(DIK_RETURN) )
	{
		m_TypingMessage = !m_TypingMessage;
	}

	if (m_Network)
	{
		if (positionChanged)
		{
			XMFLOAT3 pos = m_Player->GetPosition();
			//m_Network->SendPositionUpdate(pos.x, pos.y, pos.z, 0,0,0);
		}
	}
}

void GameState::dRender()
{
	Render3D();
	Render2D();
}

void GameState::Render3D()
{
	D3D()->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	XMMATRIX worldMatrix, translateMatrix;
	XMMATRIXIdentity(&worldMatrix);

	// Cache camera;
	m_Player->Camera()->RebuildView();

	D3D()->TurnOffCulling();
	D3D()->TurnZBufferOff();
	D3D()->TurnOnAlphaBlending();
	m_SkyBox->Draw( D3D()->GetDeviceContext(), m_Player->Camera() );
	D3D()->TurnZBufferOn();
	D3D()->TurnOffAlphaBlending();
	D3D()->TurnOnCulling();

	// Put in error checking to see the bump map has been set but no light is passed
	m_Floor->Render( D3D()->GetDeviceContext(), g_Engine->ShaderManager(), m_Player->Camera(), m_Light);

	m_DaveDude->Render( D3D()->GetDeviceContext(), g_Engine->ShaderManager(), m_Player->Camera(), m_Light, 0);

	// Render all network entities.
	for (int i = 0; i < MAX_ENTITIES; i++)
	{
		if (m_NetworkEntityList[i].IsOnline())
		{	
			if (m_NetworkEntityList[i].GetEntityType() == ENTITY_TYPE_USER)
			{
				m_NetworkEntityList->Render(m_Player->Camera(), g_Engine->ShaderManager());
			}
		}
	}

	D3D()->TurnOnAlphaBlending();
	m_BillBoard->NO_GSBillboard( m_Player->Camera() );	
	m_BillBoard->Render( D3D()->GetDeviceContext(), g_Engine->ShaderManager(), m_Player->Camera(), NULL);
	// Put the particle system vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_ParticleSystem->NO_GSBillboard( m_Player->Camera() );
	m_ParticleSystem->Render(D3D()->GetDeviceContext());

	// Render the model using the texture shader.
	g_Engine->ShaderManager()->RenderTextureShader(D3D()->GetDeviceContext(), m_ParticleSystem->GetIndexCount(), m_ParticleSystem->GetWorld(), m_Player->Camera()->GetViewMatrix(), m_Player->Camera()->GetProjMatrix(), 
		m_ParticleSystem->GetTexture());
}

void GameState::Render2D()
{
	D3D()->TurnZBufferOff();
	D3D()->TurnOffAlphaBlending();

	m_GameUI->Render( D3D(), g_Engine->ShaderManager(),  D3D()->GetOrthoMatrix());
	
	D3D()->TurnOffAlphaBlending();
	D3D()->TurnZBufferOn();
}

// Networking functions

void GameState::AddEntity(unsigned short entID, char entType, XMFLOAT3 position, XMFLOAT3 rotation)
{
	g_Engine->DebugOutput(L"Network entity added\n");
	int i = 0;

	while ((m_NetworkEntityList[i].IsOnline() == true) && (i < MAX_ENTITIES))
		i++;

	if (i == MAX_ENTITIES)
	{
		// Log out, max clients.
	}
	else
	{
		// Add the entity to the game scene
		if (!m_NetworkEntityList[i].Init(D3D(), "Data/Models/cube.txt", L"Data/Textures/marble.dds"))
			MessageBox(g_Engine->GetWindow(), L"Could not init network entity model", L"NetworkEntity Init", MB_OK);
		
		m_NetworkEntityList[i].SetOnlineStatus(true);
		m_NetworkEntityList[i].SetID(entID);
		m_NetworkEntityList[i].SetType(entType);
		m_NetworkEntityList[i].SetPosition(position);
		m_NetworkEntityList[i].SetRotation(rotation);
	}
}

void GameState::RemoveEntity(unsigned short entID)
{
	int i = 0;
	bool found = false;

	while (!found)
	{
		if ( (m_NetworkEntityList[i].GetID() == entID) && (m_NetworkEntityList[i].GetEntityType() == ENTITY_TYPE_USER))
		{
			// remove the entity from the game world.
			m_NetworkEntityList[i].SetOnlineStatus( false );
			found = true;
		}
		else
			i++;
	}
}

bool GameState::GetStateChange(char& state)
{
	if (m_stateChange)
	{
		state = m_newState;
		m_stateChange = false;
		return true;
	}
	else
	{
		return false;
	}
}

void GameState::UpdateEntityState(unsigned short entID, char state)
{
	int i = 0;
	bool found = false;

	while(!found)
	{
		if (( m_NetworkEntityList[i].GetID() == entID) && (m_NetworkEntityList[i].GetEntityType() == ENTITY_TYPE_USER))
			found = true;
		else
			i++;
	}

	// Update the current state of the entity
	m_NetworkEntityList[i].UpdateState(state);
}

bool GameState::PositionUpdate(float& posX, float& posY, float& posZ, float& rotX, float& rotY, float& rotZ)
{
	if (m_positionUpdateReady)
	{
		m_Player->GetPosition(posX, posY, posZ);
		//m_Player->GetRotation(rotX, rotY, rotZ);
		m_positionUpdateReady = false;
		return true;
	}
	else
		return false;
}

void GameState::UpdateEntityPosition(unsigned short entID, XMFLOAT3& pos, XMFLOAT3& rot, XMFLOAT3& vel, XMFLOAT3& acc, unsigned long& timestamp)
{
	int i = 0;
	bool found = false;

	while(!found)
	{
		if ((m_NetworkEntityList[i].GetID() == entID) && (m_NetworkEntityList[i].GetEntityType() == ENTITY_TYPE_USER))
			found = true;
		else
			i++;
	}

	// Update the position of the entity in the game world.
	m_NetworkEntityList[i].HasRecievedMessage(true);
	m_NetworkEntityList[i].UpdatePosition( pos, rot, vel, acc , timestamp);
}

void GameState::HandleNewPositionData(float x, float y, float z)
{
	m_Player->SetPosition(x,y,z);
	g_Engine->DebugOutput(L"New Position sent by server, setting..");
}