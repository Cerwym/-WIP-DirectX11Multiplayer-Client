#include "Engine.h"

D3DInput::D3DInput( HWND hwnd)
{
	m_hwnd = hwnd;

	// Create a DirectInput Interface
	DirectInput8Create( GetModuleHandle ( NULL ), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_DirectInput, NULL );

	// Create, prepare and acquire the keyboard and mouse devices.
	m_DirectInput->CreateDevice( GUID_SysKeyboard, &m_Keyboard, NULL );
	m_Keyboard->SetDataFormat( &c_dfDIKeyboard );

	// Cooperation level is important as it indicates how the device will share input with other applications;
	// DISCL_NONEXCLUSIVE	- Application shares access to the device.
	// DISCL_EXCLUSIVE		- Applications has exclusive access to the device.
	// DISCL_FOREGROUND		- D3DInput is only received when the application has focus.
	// DISCL_BACKGROUND		- D3DInput can be received all the time, regardless of focus.
	m_Keyboard->SetCooperativeLevel( m_hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE );
	m_Keyboard->Acquire();

	m_DirectInput->CreateDevice( GUID_SysMouse, &m_Mouse, NULL );
	m_Mouse->SetDataFormat( &c_dfDIMouse );
	m_Mouse->SetCooperativeLevel( m_hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE );
	m_Mouse->Acquire();

	m_framePress = 0;
}

D3DInput::~D3DInput()
{
	// Use the macro to release and nullify the COM objects
	S_RELEASE( m_DirectInput );
	S_RELEASE( m_Keyboard );
	S_RELEASE( m_Mouse );
}

void D3DInput::Update()
{
	static HRESULT result;

	// Poll the keyboard until it success or returns an error
	while( true )
	{
		m_Keyboard->Poll();
		if ( SUCCEEDED( result = m_Keyboard->GetDeviceState( 256, (LPVOID)&m_KeyState ) ) )
			break;
		if (result != DIERR_INPUTLOST && result != DIERR_NOTACQUIRED )
			return;

		// Reacquire the device if the focus was lost.
		if( FAILED ( m_Keyboard->Acquire() ) )
			return;
	}

	// Poll the mouse until it succeeds or returns an unknown error.
	while( true )
	{
		m_Mouse->Poll();
		if( SUCCEEDED( result = m_Mouse->GetDeviceState( sizeof( DIMOUSESTATE ), &m_mouseState ) ) )
			break;
		if( result != DIERR_INPUTLOST && result != DIERR_NOTACQUIRED )
			return;

		// Reacquire the device if the focus was lost.
		if( FAILED( m_Mouse->Acquire() ) )
			return;
	}

	// Get the relative position of the mouse.
	GetCursorPos( &m_Position );
	ScreenToClient( m_hwnd, &m_Position );

	// Increment the press stamp.
	m_framePress++;
}

//-----------------------------------------------------------------------------
// Returns true if the given key is pressed.
// Note: Consistent presses will return false when using the press stamp.
//-----------------------------------------------------------------------------
bool D3DInput::isKeyPressed( char key, bool ignoreFramePress )
{
	if( ( m_KeyState[key] & 0x80 ) == false )
		return false;

	bool pressed = true;

	if( ignoreFramePress == false )
		if( m_keyPresses[key] == m_framePress - 1 || m_keyPresses[key] == m_framePress )
			pressed = false;

	m_keyPresses[key] = m_framePress;

	return pressed;
}

//-----------------------------------------------------------------------------
// Returns true if the given button is pressed.
// Note: Consistent presses will return false when using the press stamp.
//-----------------------------------------------------------------------------
bool D3DInput::isMBPressed( char button, bool ignoreFramePress )
{
	if( ( m_mouseState.rgbButtons[button] & 0x80 ) == false )
		return false;

	bool pressed = true;

	if( ignoreFramePress == false )
		if( m_buttonPresses[button] == m_framePress - 1 || m_buttonPresses[button] == m_framePress )
			pressed = false;

	m_buttonPresses[button] = m_framePress;

	return pressed;
}