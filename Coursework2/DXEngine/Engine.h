#ifndef ENGINE_H
#define ENGINE_H

#define DIRECTINPUT_VERSION 0x0800 // Default DirectInput to version 8

#include <stdio.h>
#include <tchar.h>
#include <vector>

#include "Defines.h"
#include "D3DSys.h"
#include "D3DShaderManager.h"
#include "D3DAudio.h"
#include "D3DInput.h"
#include "State.h"
#include "LinkedList.h"

using namespace std;

struct EngineDefinition
{
	HINSTANCE hInstance; // A handle to the application's instance.
	LPCWSTR name; // The name of the application.
	void (*StateInit)(); // State Init function.
	int scrWidth; // The applications' initial screen width
	int scrHeight; // The applications' initial screen height
	bool vSync; // Is Vsync enabled or not
	bool fullscreen; // is fullscreen enabled or not
	int mouseSensitivityX; // The sensitivity of the mouse in the X axis
	int mouseSensitivityY; // The sensitivity of the mouse in the X axis

	EngineDefinition()
	{
		hInstance = NULL;
		name = L"NoNameApp";
		StateInit = NULL;
		scrWidth = 800;
		scrHeight = 600;
		vSync = false;
		fullscreen = false;
		mouseSensitivityX = 1;
		mouseSensitivityY = 1;
	}
};

class Engine
{
public:
	Engine( EngineDefinition *def = 0 );
	virtual ~Engine();

	void Run();

	HWND GetWindow(){ return m_window; }
	void SetActiveFlag( bool flag ){ m_isActive = flag; }

	void AddState( State* state, bool change = true );
	void PopState( State* state);
	void ChangeState( unsigned long id );
	void DebugOutput(WCHAR* szFormat, ...);

	// Calling this function every frame will clip the mouse's movement to the centre of the sceen so it does not escape the bounds of the window
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

	EngineDefinition *m_Definition;
	State *m_currentState; // Pointer to the current state.
	bool m_stateChanged; // Indicates if the state changed in the current frame.

	LinkedList< State > *m_States;

	D3DSys* m_D3D;
	D3DShaderManager* m_ShaderManager;
	D3DInput* m_Input;
	D3DAudio* m_Audio;
	int m_ScreenWidth, m_ScreenHeight;

};
extern Engine *g_Engine;

#endif