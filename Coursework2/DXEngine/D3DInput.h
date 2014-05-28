#pragma once

#include <dinput.h>

class D3DInput
{
public:
	D3DInput( HWND hwnd );
	virtual ~D3DInput();

	void Update();

	bool isKeyPressed( char key, bool ignoreFramePress = false );
	bool isMBPressed( char button, bool ignoreFramePress = false );

	// I want to change this to a struct<float,float> eventually.
	// Get the current position of the mouse in the X axis.
	long GetPosX(){ return m_Position.x; }
	// Get the current position of the mouse in the Y axis.
	long GetPosY(){ return m_Position.y; }
	// Get the rate of change in the mouse's x movement.
	long GetDeltaX(){ return m_mouseState.lX; }
	// Get the rate of change in the mouse's y movement.
	long GetDeltaY(){ return m_mouseState.lY; }
	// Get the rate of change if the user has a scroll wheel on their mouse
	long GetDeltaWheel(){ return m_mouseState.lZ; }
	
	// Function should only be called when each individual key is to be read. Simple checks don't need to be performed this way.
	void UpdateKeys();
	bool GetNewKeyPress(int &key);

private:
	HWND m_hwnd; // Handle to the application window
	IDirectInput8* m_DirectInput;
	unsigned long m_framePress; // Current press stamp, 'Stamping' will determine whether not the key has been pressed for multiple frames.

	IDirectInputDevice8* m_Keyboard;
	char m_KeyState[256];
	int m_newKeyPressed;
	bool m_keyPressed, m_shiftDown;
	unsigned long m_keyPresses[256]; // stores the last frame each key was pressed.

	IDirectInputDevice8* m_Mouse;
	DIMOUSESTATE m_mouseState;
	unsigned long m_buttonPresses[3]; // stores the last frame each mouse button was pressed
	POINT m_Position;
};


