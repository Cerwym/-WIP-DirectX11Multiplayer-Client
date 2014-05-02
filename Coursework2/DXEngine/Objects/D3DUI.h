#ifndef D3DUI_H
#define D3DUI_H

#include "D3DBitmap.h"
#include "..\D3DSys.h"
#include "..\D3DInput.h"
#include "..\D3DShaderManager.h"
#include "..\D3DText.h"

class D3DUI
{
public:
	//
	D3DUI();
	~D3DUI();

	bool Init(D3DSys* d3d, int scrWidth, int scrHeight, int chatWidth, int chatHeight, D3DXMATRIX baseView);
	bool InitChat(D3DSys* d3d);


	//bool Update();
	void SetPosition(int x, int y);
	void Render(ID3D11DeviceContext* context, D3DShaderManager* sMgr, D3DXMATRIX orthoMatrix);

private:

	int m_WindowXPos, m_WindowYPos;
	D3DBitmap* m_ChatWindowBackground;

	D3DXMATRIX m_baseViewMatrix;
	D3DText* m_Text;
	int m_TextEntryAreaPos;

	char m_chatBarString[50];
	char m_chatString1[50];
	char m_chatString2[50];
	char m_chatString3[50];
	char m_chatString4[50];
	char m_chatString5[50];
	char m_chatString6[50];
	char m_chatString7[50];
	bool m_newMessage;
	char m_serverMessage[50];

};

#endif // !D3DUI_H