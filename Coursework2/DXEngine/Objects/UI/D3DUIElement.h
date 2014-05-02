#ifndef D3DUIELEMENT_H
#define D3DUIELEMENT_H

#include "..\D3DBitmap.h"
#include "..\..\D3DInput.h"
#include "..\..\Defines.h"

class D3DUIElement
{

public:
	D3DUIElement();
	~D3DUIElement();
	// Init
	bool Init( ID3D11Device* device, WCHAR* bitmapFName, int bitmapWidth, int bitmapHeight, int screenWidth = 800, int screenHeight = 600 );
	// Draw
	// Is Clicked
	bool IsClicked( D3DInput* m_Input );
	// ISVisible boolean
	bool IsVisble;

protected:

private:

	// UI elements will need
	// position
	// scale
	// rotation
	// texture data
	D3DBitmap* m_TexureData;
	// active flag
	// clicked on flag


};

#endif