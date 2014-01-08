#ifndef DEFINES_H
#define DEFINES_H

// Safely delete an object and nullify its pointer
#define S_DELETE( x )	{ if ( x ) { delete ( x );		( x ) = NULL; } }
// Safely release a COM and nullify its pointer
#define S_RELEASE( x )	{ if ( x ) { ( x)->Release();	( x ) = NULL; } }
// Safely delete an array and its contents
#define S_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }

#define MAINMENU_STATE 0
#define GAME_STATE 1

#endif
