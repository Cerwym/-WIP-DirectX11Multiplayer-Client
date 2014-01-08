#ifndef ENGINEDEFINITION_H 
#define ENGINEDEFINITION_H

#include <windows.h>

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

#endif