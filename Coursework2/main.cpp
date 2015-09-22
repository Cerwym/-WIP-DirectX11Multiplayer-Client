#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <shellapi.h>

#include "DXEngine/Engine.h"
#include "DXEngine/Defines.h"
#include "MainMenuState.h"
#include "OptionsState.h"
#include "GameState.h"

void ApplicationSetup()
{
	g_Engine->AddState( new MainMenuState, true );
	g_Engine->AddState( new OptionsState, false );
	g_Engine->AddState( new GameState, false );
}

int WINAPI WinMain(  _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd )
{
	LPWSTR *szArgList;
	int argCount;

	szArgList = CommandLineToArgvW(GetCommandLine(), &argCount);

	EngineDefinition eDef;
	eDef.hInstance = hInstance;
	eDef.name = L"DirectX 11 - Multiplayer Client";
	eDef.vSync = false;
	eDef.server_addr = "127.0.0.1";

	eDef.server_port = 5555;
	eDef.StateInit = ApplicationSetup;
	eDef.mouseSensitivityX = 5;
	eDef.mouseSensitivityY = 5;
	new Engine( &eDef );
	g_Engine->Run();

	// Free memory allocated for CommandLineToArtgvW arguments
	LocalFree(szArgList);

	return 0;
}