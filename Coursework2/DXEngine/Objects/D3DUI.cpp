#include "D3DUI.h"

// construct m_D3D either
D3DUI::D3DUI()
{
	m_ChatWindowBackground = 0;
	m_Text = 0;
}

D3DUI::~D3DUI()
{
	if (m_Text)
	{
		delete m_Text;
		m_Text = 0;
	}

	if (m_ChatWindowBackground)
	{
		delete m_ChatWindowBackground;
		m_ChatWindowBackground = 0;
	}
}

bool D3DUI::Init(D3DSys* d3d, int scrWidth, int scrHeight, int chatWidth, int chatHeight, D3DXMATRIX baseView)
{
	m_baseViewMatrix = baseView;

	//m_WindowXPos = ((scrWidth / 2) - 300);
	//m_WindowYPos = (scrHeight - 215);

	m_ChatWindowBackground = new D3DBitmap;
	if (!m_ChatWindowBackground)
		return false;

	if (!m_ChatWindowBackground->Init(d3d->GetDevice(), scrWidth, scrHeight, L"Data/Textures/ChatWindow.dds", chatWidth, chatHeight))
		return false;

	m_Text = new D3DText;
	if (!m_Text)
		return false;

	if (!m_Text->Init(d3d->GetDevice(), d3d->GetDeviceContext(), 0, scrWidth, scrHeight,baseView ))
		return false;

	if (!InitChat(d3d))
		return false;

	m_TextEntryAreaPos = 0;

	m_chatBarString[0] = '\0';
	m_chatString1[0] = '\0';
	m_chatString2[0] = '\0';
	m_chatString3[0] = '\0';
	m_chatString4[0] = '\0';
	m_chatString5[0] = '\0';
	m_chatString6[0] = '\0';
	m_chatString7[0] = '\0';

	m_newMessage = false;

	return true;
}

void D3DUI::SetPosition(int x, int y)
{
	// Move all elements that have a position value;
	if (m_ChatWindowBackground)
		m_ChatWindowBackground->SetPosition(x, y);
}


bool D3DUI::InitChat(D3DSys* d3d)
{
	bool result;


	// Chat string 1.
	result = m_Text->CreateSentence(device, 6, 50, " ", (m_chatWindowX + 20), (m_chatWindowY + 20), 1.0f, 1.0f, 0.0f);
	if(!result)
	{
		return false;
	}

	// Chat string 2.
	result = m_Text->CreateSentence(device, 7, 50, " ", (m_chatWindowX + 20), (m_chatWindowY + 40), 1.0f, 1.0f, 0.0f);
	if(!result)
	{
		return false;
	}

	// Chat string 3.
	result = m_Text->CreateSentence(device, 8, 50, " ", (m_chatWindowX + 20), (m_chatWindowY + 60), 1.0f, 1.0f, 0.0f);
	if(!result)
	{
		return false;
	}

	// Chat string 4.
	result = m_Text->CreateSentence(device, 9, 50, " ", (m_chatWindowX + 20), (m_chatWindowY + 80), 1.0f, 1.0f, 0.0f);
	if(!result)
	{
		return false;
	}

	// Chat string 5.
	result = m_Text->InitSentence(device, 10, 50, " ", (m_chatWindowX + 20), (m_chatWindowY + 100), 1.0f, 1.0f, 0.0f);
	if(!result)
	{
		return false;
	}

	// Chat string 6.
	result = m_Text->InitSentence(device, 11, 50, " ", (m_chatWindowX + 20), (m_chatWindowY + 120), 1.0f, 1.0f, 0.0f);
	if(!result)
	{
		return false;
	}

	// Chat string 7.
	result = m_Text->CreateSentence(device, 12, 50, " ", (m_chatWindowX + 20), (m_chatWindowY + 140), 1.0f, 1.0f, 0.0f);
	if(!result)
	{
		return false;
	}

	// Talk bar string.
	result = m_Text->CreateSentence(device, 13, 50, " ", (m_chatWindowX + 20), (m_chatWindowY + 180), 1.0f, 1.0f, 1.0f);
	if(!result)
	{
		return false;
	}

}

void D3DUI::Render(ID3D11DeviceContext* context, D3DShaderManager* sMgr, D3DXMATRIX orthoMatrix)
{
	m_ChatWindowBackground->Render(context, sMgr, m_baseViewMatrix, orthoMatrix);
}