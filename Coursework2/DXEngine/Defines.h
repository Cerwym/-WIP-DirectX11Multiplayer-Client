#pragma once

// Safely delete an object and nullify its pointer
#define S_DELETE( x )	{ if ( x ) { delete ( x );		( x ) = NULL; } }
// Safely release a COM and nullify its pointer
#define S_RELEASE( x )	{ if ( x ) { ( x)->Release();	( x ) = NULL; } }
// Safely delete an array and its contents
#define S_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }

struct Vector2
{
	float x, y, z;
};

#define LOADING_STATE 0
#define MAINMENU_STATE 1
#define OPTIONS_STATE 2
#define GAME_STATE 3

