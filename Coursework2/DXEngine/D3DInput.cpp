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

void D3DInput::UpdateKeys()
{
	for (int i = 0; i < 256; i++)
	{
		if (m_KeyState[i] & 0x80)
		{
			// If this key has been pressed, set it as the new key and update the state
			if (m_keyPresses[i] == false)
			{
				m_newKeyPressed = i;
				m_keyPressed = true;
				m_keyPresses[i] = true;
			}
		}

		// Otherwise set this index to be unchanged
		else
		{
			m_keyPresses[i] = false;
		}
	}

	// Check to see if either shift key has been pressed
	if ((m_KeyState[DIK_LSHIFT] & 0x80) || (m_KeyState[DIK_RSHIFT] & 0x80))
		m_shiftDown = true;
	else
		m_shiftDown = false;
}

bool D3DInput::GetNewKeyPress(int &key)
{
	key = 0;

	if (!m_keyPressed)
		return false;
	m_keyPressed = false;

	if (m_newKeyPressed == DIK_BACK)
	{
		key = 8;
		return true;
	}

	if (m_newKeyPressed == DIK_RETURN)
	{
		key = 13;
		return true;
	}

	if (m_newKeyPressed == DIK_SPACE)
	{
		key = 32;
		return true;
	}

	if (m_newKeyPressed == DIK_F1)
	{
		key = DIK_F1;
		return true;
	}

	if (m_newKeyPressed == DIK_F2)
	{
		key = DIK_F2;
		return true;
	}

	// The keys without the shift key pressed.
	if(!m_shiftDown)
	{
		switch(m_newKeyPressed)
		{
		case DIK_APOSTROPHE: { key = 39;  break; }
		case DIK_COMMA:      { key = 44;  break; }
		case DIK_MINUS:      { key = 45;  break; }
		case DIK_PERIOD:     { key = 46;  break; }
		case DIK_SLASH:      { key = 47;  break; }
		case DIK_0:          { key = 48;  break; }
		case DIK_1:          { key = 49;  break; }
		case DIK_2:          { key = 50;  break; }
		case DIK_3:          { key = 51;  break; }
		case DIK_4:          { key = 52;  break; }
		case DIK_5:          { key = 53;  break; }
		case DIK_6:          { key = 54;  break; }
		case DIK_7:          { key = 55;  break; }
		case DIK_8:          { key = 56;  break; }
		case DIK_9:          { key = 57;  break; }
		case DIK_SEMICOLON:  { key = 59;  break; }
		case DIK_EQUALS:     { key = 61;  break; }		
		case DIK_LBRACKET:   { key = 91;  break; }
		case DIK_BACKSLASH:  { key = 92;  break; }
		case DIK_RBRACKET:   { key = 93;  break; }
		case DIK_GRAVE:      { key = 96;  break; }
		case DIK_A:          { key = 97;  break; }
		case DIK_B:          { key = 98;  break; }
		case DIK_C:          { key = 99;  break; }
		case DIK_D:          { key = 100; break; }
		case DIK_E:          { key = 101; break; }
		case DIK_F:          { key = 102; break; }
		case DIK_G:          { key = 103; break; }
		case DIK_H:          { key = 104; break; }
		case DIK_I:          { key = 105; break; }
		case DIK_J:          { key = 106; break; }
		case DIK_K:          { key = 107; break; }
		case DIK_L:          { key = 108; break; }
		case DIK_M:          { key = 109; break; }
		case DIK_N:          { key = 110; break; }
		case DIK_O:          { key = 111; break; }
		case DIK_P:          { key = 112; break; }
		case DIK_Q:          { key = 113; break; }
		case DIK_R:          { key = 114; break; }
		case DIK_S:          { key = 115; break; }
		case DIK_T:          { key = 116; break; }
		case DIK_U:          { key = 117; break; }
		case DIK_V:          { key = 118; break; }
		case DIK_W:          { key = 119; break; }
		case DIK_X:          { key = 120; break; }
		case DIK_Y:          { key = 121; break; }
		case DIK_Z:          { key = 122; break; }
		default:             { key = 0;   break; }
		}
	}

	// The keys with the shift key pressed.
	if(m_shiftDown)
	{
		switch(m_newKeyPressed)
		{
		case DIK_1:          { key = 33;  break; }
		case DIK_APOSTROPHE: { key = 34;  break; }
		case DIK_3:          { key = 35;  break; }
		case DIK_4:          { key = 36;  break; }
		case DIK_5:          { key = 37;  break; }
		case DIK_7:          { key = 38;  break; }
		case DIK_9:          { key = 40;  break; }
		case DIK_0:          { key = 41;  break; }
		case DIK_8:          { key = 42;  break; }
		case DIK_EQUALS:     { key = 43;  break; }
		case DIK_SEMICOLON:  { key = 58;  break; }
		case DIK_COMMA:      { key = 60;  break; }
		case DIK_PERIOD:     { key = 62;  break; }
		case DIK_SLASH:      { key = 63;  break; }
		case DIK_2:          { key = 64;  break; }
		case DIK_A:          { key = 65;  break; }
		case DIK_B:          { key = 66;  break; }
		case DIK_C:          { key = 67;  break; }
		case DIK_D:          { key = 68;  break; }
		case DIK_E:          { key = 69;  break; }
		case DIK_F:          { key = 70;  break; }
		case DIK_G:          { key = 71;  break; }
		case DIK_H:          { key = 72;  break; }
		case DIK_I:          { key = 73;  break; }
		case DIK_J:          { key = 74;  break; }
		case DIK_K:          { key = 75;  break; }
		case DIK_L:          { key = 76;  break; }
		case DIK_M:          { key = 77;  break; }
		case DIK_N:          { key = 78;  break; }
		case DIK_O:          { key = 79;  break; }
		case DIK_P:          { key = 80;  break; }
		case DIK_Q:          { key = 81;  break; }
		case DIK_R:          { key = 82;  break; }
		case DIK_S:          { key = 83;  break; }
		case DIK_T:          { key = 84;  break; }
		case DIK_U:          { key = 85;  break; }
		case DIK_V:          { key = 86;  break; }
		case DIK_W:          { key = 87;  break; }
		case DIK_X:          { key = 88;  break; }
		case DIK_Y:          { key = 89;  break; }
		case DIK_Z:          { key = 90;  break; }
		case DIK_6:          { key = 94;  break; }
		case DIK_MINUS:      { key = 95;  break; }
		case DIK_LBRACKET:   { key = 123; break; }
		case DIK_BACKSLASH:  { key = 124; break; }
		case DIK_RBRACKET:   { key = 125; break; }
		case DIK_GRAVE:      { key = 126; break; }
		default:             { key = 0;   break; }
		}
	}

	return true;
}