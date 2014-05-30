#pragma once

#define DIRECTINPUT_VERSION 0x0800 // Default DirectInput to version 8

#include <stdio.h>
#include <tchar.h>
#include <vector>
#include <thread>
#include <future>

#include "EngineDefinition.h"
#include "Defines.h"
#include "D3DSys.h"
#include "D3DShaderManager.h"
#include "D3DAudio.h"
#include "D3DInput.h"
#include "State.h"
#include "LinkedList.h"

// Change this to be contained in DXENGINE
#include "..\LoadingState.h"

using namespace std;

LRESULT CALLBACK WindowProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam );

class Engine
{
public:
	Engine( EngineDefinition *def = 0 );
	virtual ~Engine();

	void Run();

	HWND GetWindow(){ return m_window; }
	void SetActiveFlag( bool flag ){ m_isActive = flag; }
	bool GetActiveFlag(){return m_isActive;}

	void AddState( State* state, bool change = true );
	void PopState( State* state);
	void ChangeState( unsigned long id );
	void ThreadedLoadState( State* stateToLoad );
	void DebugOutput(WCHAR* szFormat, ...);

	// Calling this function every frame will clip the mouse's movement to the center of the screen so it does not escape the bounds of the window
	void LockMouseToCentre();

	State* GetState(){ return m_currentState; }
	D3DInput* GetInput(){ return m_Input; }
	D3DAudio* GetAudio(){ return m_Audio; }
	D3DSys* Device(){ return m_D3D; }
	D3DShaderManager* ShaderManager(){ return m_ShaderManager; }
	int GetScreenWidth(){ return m_Definition->scrWidth; }
	int GetScreenHeight(){ return m_Definition->scrHeight; }
	int GetMouseSensitivityX() { return m_Definition->mouseSensitivityX; }
	int GetMouseSensitivityY() { return m_Definition->mouseSensitivityY; }

private:
	bool m_loaded; // Indicates if the engine is loading
	HWND m_window; // A handle to the main window
	RECT m_windowRect; // A rectangle to represent the bounds of the window
	bool m_isActive;

	EngineDefinition* m_Definition;
	State* m_currentState; // Pointer to the current state.
	State* m_previousState; // Pointer to the last known state
	State* m_loadingState;
	bool m_stateChanged; // Indicates if the state changed in the current frame.

	LinkedList< State > *m_States;

	D3DSys* m_D3D;
	D3DShaderManager* m_ShaderManager;
	D3DInput* m_Input;
	D3DAudio* m_Audio;
	int m_ScreenWidth, m_ScreenHeight;

};
extern Engine *g_Engine;

