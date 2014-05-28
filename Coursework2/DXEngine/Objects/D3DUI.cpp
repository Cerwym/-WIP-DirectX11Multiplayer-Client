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

bool D3DUI::Init(D3DSys* d3d, int scrWidth, int scrHeight, int chatWidth, int chatHeight, D3DXMATRIX baseView, int posY, int posX)
{
	m_baseViewMatrix = baseView;
	m_context = d3d->GetDeviceContext();

	m_WindowXPos = posX;
	m_WindowYPos = posY;

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

bool D3DUI::AddChatMessage()
{
	strcpy_s(m_chatString1, m_chatString2);
	strcpy_s(m_chatString2, m_chatString3);
	strcpy_s(m_chatString3, m_chatString4);
	strcpy_s(m_chatString4, m_chatString5);
	strcpy_s(m_chatString5, m_chatString6);
	strcpy_s(m_chatString6, m_chatString7);
	strcpy_s(m_chatString7, "You say: ");
	strcat_s(m_chatString7, m_chatBarString);

	// Should really check to see the return of these calls.
	// Update the sentence vertex buffer with the new string information.
	m_Text->UpdateSentence(6, m_chatString1, (m_WindowXPos + 20), (m_WindowYPos + 0), 1.0f, 1.0f, 0.0f, m_context);
	m_Text->UpdateSentence(7, m_chatString2, (m_WindowXPos + 20), (m_WindowYPos + 20), 1.0f, 1.0f, 0.0f, m_context);
	m_Text->UpdateSentence(8, m_chatString3, (m_WindowXPos + 20), (m_WindowYPos + 40), 1.0f, 1.0f, 0.0f, m_context);
	m_Text->UpdateSentence(9, m_chatString4, (m_WindowXPos + 20), (m_WindowYPos + 60), 1.0f, 1.0f, 0.0f, m_context);
	m_Text->UpdateSentence(10, m_chatString5, (m_WindowXPos + 20), (m_WindowYPos + 80), 1.0f, 1.0f, 0.0f, m_context);
	m_Text->UpdateSentence(11, m_chatString6, (m_WindowXPos + 20), (m_WindowYPos + 100), 1.0f, 1.0f, 0.0f, m_context);
	m_Text->UpdateSentence(12, m_chatString7, (m_WindowXPos + 20), (m_WindowYPos + 120), 1.0f, 1.0f, 0.0f, m_context);

	return true;
}

void D3DUI::KeyPressUpdate(int key)
{
	if (key == 0)
		return;

	if (key == 8)
	{
		if (m_TextEntryAreaPos > 0)
		{
			m_TextEntryAreaPos--;
			m_chatBarString[m_TextEntryAreaPos] = '\0';
			UpdateChatBar();
		}
		return;
	}

	if (key == 13)
	{
		AddChatMessage();
		AddChatMessageForServer();
		m_chatBarString[0] = '\0';
		m_TextEntryAreaPos = 0;
		UpdateChatBar();
		return;
	}

	if (m_TextEntryAreaPos < 38)
	{
		m_chatBarString[m_TextEntryAreaPos] = key;
		m_TextEntryAreaPos++;
		m_chatBarString[m_TextEntryAreaPos] = '\0';
		UpdateChatBar();
	}
	return;
}

bool D3DUI::UpdateChatBar()
{
	if (!m_Text->UpdateSentence(13, m_chatBarString, (m_WindowXPos + 20), (m_WindowYPos + 140), 1.0f, 1.0f, 1.0f, m_context))
		return false;
	return true;
}

void D3DUI::CheckForChatMessage(char* message, bool &newMessage)
{
	if (m_newMessage)
	{
		newMessage = true;
		strcpy_s(message, 50, m_serverMessage);
		m_newMessage = false;
	}
	else
		newMessage = false;
}

void D3DUI::AddChatMessageForServer()
{
	m_newMessage = true;
	strcpy_s(m_serverMessage, m_chatBarString);
}

bool D3DUI::AddChatMessageFromServer(char* message, int ID)
{
	char tempString[16];
	char textString[50];

	// Convert the ID number to string format.
	_itoa_s(ID, tempString, 10);

	strcpy_s(textString, "User ");
	strcat_s(textString, tempString);
	strcat_s(textString, " says : ");
	strcat_s(textString, message);

	// 'push' all the text strings up.
	strcpy_s(m_chatString1, m_chatString2);
	strcpy_s(m_chatString2, m_chatString3);
	strcpy_s(m_chatString3, m_chatString4);
	strcpy_s(m_chatString4, m_chatString5);
	strcpy_s(m_chatString5, m_chatString6);
	strcpy_s(m_chatString6, m_chatString7);
	strcpy_s(m_chatString7, textString);

	// Update the sentence vertex buffer with the new string information.
	if (!m_Text->UpdateSentence(6, m_chatString1, (m_WindowXPos + 20), (m_WindowYPos), 1.0f, 1.0f, 0.0f, m_context))
	{
		return false;
	}
	if (!m_Text->UpdateSentence(7, m_chatString2, (m_WindowXPos + 20), (m_WindowYPos + 20), 1.0f, 1.0f, 0.0f, m_context))
	{
		return false;
	}
	
	if (!m_Text->UpdateSentence(8, m_chatString3, (m_WindowXPos + 20), (m_WindowYPos + 40), 1.0f, 1.0f, 0.0f, m_context))
	{
		return false;
	}
	
	if (!m_Text->UpdateSentence(9, m_chatString4, (m_WindowXPos + 20), (m_WindowYPos + 60), 1.0f, 1.0f, 0.0f, m_context))
		return false;

	if (!m_Text->UpdateSentence(10, m_chatString5, (m_WindowXPos + 20), (m_WindowYPos + 80), 1.0f, 1.0f, 0.0f, m_context))
		return false;

	if (!m_Text->UpdateSentence(11, m_chatString6, (m_WindowXPos + 20), (m_WindowYPos + 100), 1.0f, 1.0f, 0.0f, m_context))
		return false;

	if (!m_Text->UpdateSentence(12, m_chatString7, (m_WindowXPos + 20), (m_WindowYPos + 120), 1.0f, 1.0f, 0.0f, m_context))
		return false;

	return true;
}

void D3DUI::SetPosition(int x, int y)
{
	// Move all elements that have a position value;
	if (m_ChatWindowBackground)
	{
		m_WindowXPos = x; m_WindowYPos = y;
		m_ChatWindowBackground->SetPosition(x, y);
	}
}

bool D3DUI::InitChat(D3DSys* d3d)
{
	bool result;

	// Chat string 1.
	result = m_Text->CreateSentence(d3d, 6, 50, " ", (m_WindowXPos + 20), (m_WindowYPos + 0), 1.0f, 1.0f, 0.0f);
	if(!result)
	{
		return false;
	}

	// Chat string 2.
	result = m_Text->CreateSentence(d3d, 7, 50, " ", (m_WindowXPos + 20), (m_WindowYPos + 20), 1.0f, 1.0f, 0.0f);
	if(!result)
	{
		return false;
	}

	// Chat string 3.
	result = m_Text->CreateSentence(d3d, 8, 50, " ", (m_WindowXPos + 20), (m_WindowYPos + 40), 1.0f, 1.0f, 0.0f);
	if(!result)
	{
		return false;
	}

	// Chat string 4.
	result = m_Text->CreateSentence(d3d, 9, 50, " ", (m_WindowXPos + 20), (m_WindowYPos + 60), 1.0f, 1.0f, 0.0f);
	if(!result)
	{
		return false;
	}

	// Chat string 5.
	result = m_Text->CreateSentence(d3d, 10, 50, " ", (m_WindowXPos + 20), (m_WindowYPos + 80), 1.0f, 1.0f, 0.0f);
	if(!result)
	{
		return false;
	}

	// Chat string 6.
	result = m_Text->CreateSentence(d3d, 11, 50, " ", (m_WindowXPos + 20), (m_WindowYPos + 100), 1.0f, 1.0f, 0.0f);
	if(!result)
	{
		return false;
	}

	// Chat string 7.
	result = m_Text->CreateSentence(d3d, 12, 50, " ", (m_WindowXPos + 20), (m_WindowYPos + 120), 1.0f, 1.0f, 0.0f);
	if(!result)
	{
		return false;
	}

	// Talk bar string.
	result = m_Text->CreateSentence(d3d, 13, 50, " ", (m_WindowXPos + 20), (m_WindowYPos + 140), 1.0f, 1.0f, 1.0f);
	if(!result)
	{
		return false;
	}

}

bool D3DUI::Render(D3DSys* d3d, D3DShaderManager* sMgr, D3DXMATRIX orthoMatrix)
{
	if (!m_ChatWindowBackground->Render(d3d->GetDeviceContext(), sMgr, m_baseViewMatrix, orthoMatrix))
		return false;

	d3d->TurnOnAlphaBlending();

	if (!m_Text->Render(d3d->GetDeviceContext(), orthoMatrix))
		return false;

	d3d->TurnOffAlphaBlending();
}