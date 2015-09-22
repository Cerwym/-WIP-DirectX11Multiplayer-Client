#include "Engine.h"

Engine *g_Engine = NULL;

Engine::Engine( EngineDefinition *definition )
{
	m_ShaderManager = 0;
	m_loadingState = 0;

	WNDCLASSEX wcex; // The definition for the Window class structure.

	// The engine has not yet loaded
	m_loaded = false;

	// If no definition was passed, create a default definition
	m_Definition = new EngineDefinition;
	if (definition != NULL)
		memcpy( m_Definition, definition, sizeof( EngineDefinition ) );

	m_ScreenWidth = m_Definition->scrWidth;
	m_ScreenHeight = m_Definition->scrHeight;

	// Store a pointer to the engine
	g_Engine = this;

	wcex.cbSize = sizeof ( WNDCLASSEX );
	wcex.style = CS_CLASSDC;
	wcex.lpfnWndProc = WindowProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = m_Definition->hInstance;
	wcex.hIcon = LoadIcon( NULL, IDI_APPLICATION );
	wcex.hCursor = LoadCursor( NULL, IDC_ARROW );
	wcex.hbrBackground = NULL;
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"WindowClass";
	wcex.hIconSm = LoadIcon( NULL, IDI_APPLICATION );
	RegisterClassEx( &wcex );

	// Initialize the COM using multi threaded concurrency.
	CoInitializeEx( NULL, COINIT_MULTITHREADED );

	// Might want to check if full-screen is passed as a parameter and do specific options
	// Place the window in the middle of the screen.
	int posX = (GetSystemMetrics(SM_CXSCREEN) - m_Definition->scrWidth ) /2;
	int posY = (GetSystemMetrics(SM_CYSCREEN) - m_Definition->scrHeight) /2;

	// Create the window and retrieve a handle to it
	m_window = CreateWindowEx( WS_EX_APPWINDOW, L"WindowClass", m_Definition->name, WS_OVERLAPPED, posX, posY, m_Definition->scrWidth, m_Definition->scrHeight, NULL, NULL, m_Definition->hInstance, NULL);
	// Create a rectangle to represent the physical bounds of the window.
	GetWindowRect(m_window, &m_windowRect);

	m_States = new LinkedList< State >;
	m_currentState = NULL;
	m_previousState = NULL;

	
	m_Input = new D3DInput( m_window );
	m_Audio = 0;
	
	m_Audio = new D3DAudio;
	if (!m_Audio->Init(m_window))
	{
		MessageBox( m_window, L"Error creating the audio interface", L"Error", MB_OK);
		PostQuitMessage( 0 );
	}

	m_D3D = new D3DSys;
	if ( !m_D3D->Init( m_Definition->scrWidth, m_Definition->scrHeight, m_Definition->vSync, m_window, m_Definition->fullscreen, 0.1f, 1000.0f ) )
	{
		MessageBox( m_window, L"Could not Init Direct3D", L"Error", MB_OK );
		PostQuitMessage( 0 );
	}

	// Initialize the shader manager will all available shaders
	
	m_ShaderManager = new D3DShaderManager;
	if (!m_ShaderManager->Init(m_D3D->GetDevice(), m_window))
	{
		MessageBox( m_window, L"Could not Init the shader manager", L"Error", MB_OK );
		PostQuitMessage( 0 );
	}

	srand( timeGetTime() );

	// Allow the application perform state setup if it is required.
	if (m_Definition->StateInit != NULL )
		m_Definition->StateInit();

	// Bring the window up on the screen and set it as the main focus
	ShowWindow( m_window, SW_SHOW );
	SetForegroundWindow( m_window );
	SetFocus( m_window );
	ShowCursor(false);

	m_loaded = true;
}

Engine::~Engine()
{
	if ( m_loaded )
	{
		// Delete the states
		if ( m_currentState != NULL )
			m_currentState->Close();
		
		S_DELETE( m_States ); // The dcstor for the vector will call here.

		// Everything will be destroyed here (including DirectX COM objects).
		S_DELETE( m_Input );
		S_DELETE( m_Audio );
		S_DELETE( m_D3D );
		S_DELETE( m_ShaderManager );
	}
	
	// Uninitialize the COM
	CoUninitialize();

	DestroyWindow(m_window);
	m_window = NULL;

	UnregisterClass( L"WindowClass", m_Definition->hInstance );
	ShowCursor(true);

	S_DELETE( m_Definition );
}

void Engine::Run()
{
	if ( m_loaded == true)
	{
		ShowWindow( m_window, SW_NORMAL );

		MSG msg;
		ZeroMemory( &msg, sizeof( MSG ) );
		while( msg.message != WM_QUIT )
		{
			if (PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
			{
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}

			else 
			{
				m_Input->Update();

				// Update the current state (if there is one), taking state changes into account
				m_stateChanged = false;
				if ( m_currentState != NULL )
					m_currentState->Update( m_isActive );

				if ( m_stateChanged == true ) continue;

				// Render the current state if there is one.
				if ( m_currentState != NULL )
					m_currentState->Render();
			}
		}
	}

	S_DELETE( g_Engine );
}

void Engine::LockMouseToCentre()
{
	SetCursorPos(m_windowRect.left + (m_Definition->scrWidth /2), m_windowRect.top + (m_Definition->scrHeight /2));
}

void Engine::AddState( State* state, bool change /* = true */ )
{
	m_States->Add( state );

	if( change == false )
		return;

	if ( m_currentState != NULL )
		m_currentState->Close();

	m_currentState = m_States->GetLast();

	ThreadedLoadState(m_currentState);
}

void Engine::ThreadedLoadState( State* stateToLoad )
{
	// Set the current state to be the parameter passed, this is harmless in itself, but will be required for the recursive call.
	m_currentState = stateToLoad;

	m_loadingState = new LoadingState;
	m_loadingState->Load( m_Definition, m_D3D, m_Audio, m_Input );

	// Use a future so that the result of the state's load can be evaluated later.
	// Do this asynchronously so that the loading thread will still draw.
	std::future<bool> val = std::async(std::launch::async, &State::Load, stateToLoad, m_Definition, m_D3D, m_Audio, m_Input );

	while (true)
	{	
		// Wait for one millisecond and if a result (the state has finished or returned false) is valid, break from this loop.
		std::chrono::milliseconds timeout(1);
		if (val.wait_for(timeout) == std::future_status::ready)
		{
			break;
		}

		ShowWindow( m_window, SW_NORMAL );
		
		m_loadingState->Update( true );
		m_loadingState->Render();
	}

	// destroy the loading screen if the current state has finished loading
	m_loadingState->Close();
	delete m_loadingState;
	m_loadingState = 0;

	// Get the returned value from the threaded load.
	bool success = val.get();
	if (success == false) // If the state failed to load correctly, load the last working state.
	{
		DebugOutput(L"Loading failed, loading previous state...\n");
		// Close the state that did not load correctly.
		stateToLoad->Close();
		PopState(stateToLoad); // Pop the state that failed to load, otherwise data from that state will still exists, prohibiting the application from qutting cleanly.
		if (m_previousState != NULL)
		{
			m_previousState->SetFailed();
			ThreadedLoadState(m_previousState);
		}

		assert(m_previousState != NULL && "UNIMPLEMENTED BAILOUT. No state exists to fall back on. Application will crash");
	}

	// If we've reached here, everything will have loaded correctly, rejoice! :)
}

void Engine::PopState( State* state )
{
	m_States->Remove( &state );
}

void Engine::ChangeState( unsigned long id )
{
	m_States->Iterate( true );
	while ( m_States->Iterate() != NULL )
	{
		if ( m_States->GetCurrent()->GetID() == id )
		{
			// Close the old state;
			if ( m_currentState != NULL )
			{
				m_previousState = m_currentState; // store it as the previous
				m_currentState->Close();
			}

			m_currentState = m_States->GetCurrent();
			// Give updated pointers to the state as the context of these DirectX objects may have changed
			ThreadedLoadState(m_currentState);
			m_stateChanged = true;

			break;
		}
	}
}

void Engine::DebugOutput(WCHAR* szFormat, ...)
{
	WCHAR szBuff[1024];
	va_list arg;
	va_start(arg, szFormat);
	_vsnwprintf(szBuff, sizeof(szBuff), szFormat, arg);
	va_end(arg);

	OutputDebugString(szBuff);
}

LRESULT CALLBACK WindowProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	switch ( msg )
	{
	case WM_ACTIVATEAPP:
		g_Engine->SetActiveFlag( wParam );
		return 0;

	case WM_DESTROY:
		PostQuitMessage( 0 );
		return 0;

	default:
		return DefWindowProc( hwnd, msg, wParam, lParam );
	}
}
