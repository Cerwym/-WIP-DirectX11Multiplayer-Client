#include <windows.h>

#include "DXEngine/Engine.h"
#include "DXEngine/Defines.h"
#include "MainMenuState.h"
#include "GameState.h"

void ApplicationSetup()
{
	g_Engine->AddState( new MainMenuState, true );
	g_Engine->AddState( new GameState, false );
}

int WINAPI WinMain(  _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd )
{
	EngineDefinition eDef;
	eDef.hInstance = hInstance;
	eDef.name = L"0901632 - Coursework - Programming Games - Engine2";
	eDef.vSync = true;
	eDef.StateInit = ApplicationSetup;
	eDef.mouseSensitivityX = 5;
	eDef.mouseSensitivityY = 5;

	new Engine( &eDef );
	g_Engine->Run();

	return true;
}