#ifndef ENGINEDEFINITION_H 
#define ENGINEDEFINITION_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>

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
	char* server_addr;
	unsigned short server_port; 

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
		server_addr = NULL;
		server_port = 0;
	}
};

#endif