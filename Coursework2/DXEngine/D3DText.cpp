#include "D3DText.h"

D3DText::D3DText()
{
	m_Font = 0;
	m_FontShader = 0;
	m_sentenceArray = 0;
}

D3DText::~D3DText()
{
	S_DELETE(m_Font);
	S_DELETE(m_FontShader);

	if (m_sentenceArray)
	{
		for (int i = 0; i < MAX_SENTENCES; i++)
			if (m_sentenceArray[i].active)
				ReleaseSentence(i);

		delete [] m_sentenceArray;
		m_sentenceArray = 0;
	}
}

bool D3DText::Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext, HWND hwnd, int screenWidth, int screenHeight, XMMATRIX baseViewMatrix)
{
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// Store the base view matrix.
	m_baseViewMatrix = baseViewMatrix;
	XMMATRIXIdentity(&m_worldMatrix);

	m_Font = new D3DFont;
	if(!m_Font)
		return false;

	// Initialize the font object.
	if(!m_Font->Init(device, "Data/Fonts/font_1.txt", L"Data/Fonts/font_1.dds") )
	{
		MessageBox(hwnd, L"Could not initialize the font object.", L"Error", MB_OK);
		return false;
	}

	m_FontShader = new D3DFontShader;
	if(!m_FontShader)
		return false;

	// Initialize the font shader object.
	if(!m_FontShader->Init(device, hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the font shader object.", L"Error", MB_OK);
		return false;
	}
	
	m_sentenceArray = new SentanceType[MAX_SENTENCES];
	if (!m_sentenceArray)
		return false;

	for (int i = 0; i < MAX_SENTENCES; i ++)
		m_sentenceArray[i].active = false;

	/*

	// Initialize the first sentence.
	if(!InitSentence(&m_sentence1, 16, device))
		return false;

	// Now update the sentence vertex buffer with the new string information.
	if(!UpdateSentence(m_sentence1, "Mouse X : ", 0, 0, 1.0f, 1.0f, 1.0f, deviceContext))
		return false;

	// Initialize the second sentence.
	if(!InitSentence(&m_sentence2, 16, device))
		return false;

	// Now update the sentence vertex buffer with the new string information.
	if(!UpdateSentence(m_sentence2, "Mouse Y : ", 0, 15, 1.0f, 1.0f, 0.0f, deviceContext))
		return false;

	if(!InitSentence(&CPUText, 16, device))
		return false;
	if(!UpdateSentence(CPUText, "CPU: 00", 0, 50, 1.0f, 1.0f, 0.0f, deviceContext))
		return false;

	if(!InitSentence(&FPSText, 16, device))
		return false;

	if(!UpdateSentence(FPSText, "F p s: 00", 0, 65, 1.0f, 1.0f, 0.0f, deviceContext))
		return false;

	*/

	return true;
}

bool D3DText::Render(ID3D11DeviceContext* deviceContext, XMMATRIX orthoMatrix)
{
	/*
	//Todo change this to draw all sentences in a given vector instead of m_sentenceX
	if(!RenderSentence(deviceContext, m_sentence1, orthoMatrix))
		return false;

	// Draw the second sentence.
	if(!RenderSentence(deviceContext, m_sentence2, orthoMatrix))
		return false;

	// Draw the FPS sentence.
	if(!RenderSentence(deviceContext, FPSText, orthoMatrix))
		return false;
	
	// Draw the second sentence.
	if(!RenderSentence(deviceContext, CPUText, orthoMatrix))
		return false;

	*/

	for (int i = 0; i < MAX_SENTENCES; i++)
	{
		if( m_sentenceArray[i].active)
		{
			if (!RenderSentence(deviceContext, i, orthoMatrix))
				return false;
		}
	}

	return true;
}

bool D3DText::InitSentence(int ID, int maxLength, ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	m_sentenceArray[ID].active = true;
	m_sentenceArray[ID].vertexBuffer = 0;
	m_sentenceArray[ID].indexBuffer = 0;
	m_sentenceArray[ID].maxLength = maxLength;
	m_sentenceArray[ID].vertexCount = 6 * maxLength;
	m_sentenceArray[ID].indexCount = m_sentenceArray[ID].vertexCount;

	vertices = new VertexType[m_sentenceArray[ID].vertexCount];
	if(!vertices)
		return false;

	indices = new unsigned long[m_sentenceArray[ID].indexCount];
	if(!indices)
		return false;

	// Initialize vertex array to zeros at first.
	memset(vertices, 0, (sizeof(VertexType) * m_sentenceArray[ID].vertexCount));

	// Initialize the index array.
	for(int i = 0; i < m_sentenceArray[ID].indexCount; i++)
		indices[i] = i;

	//During the creation of the vertex buffer description for the sentence we set the Usage type to dynamic as we may want to change the contents of the sentence at any time.
	// Set up the description of the dynamic vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_sentenceArray[ID].vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Create the vertex buffer.
	if(FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_sentenceArray[ID].vertexBuffer)))
		return false;
	
	// The index buffer is setup as a normal static buffer since the contents will never need to change.
	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_sentenceArray[ID].indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	if(FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &m_sentenceArray[ID].indexBuffer)))
		return false;

	delete [] vertices;
	vertices = 0;

	delete [] indices;
	indices = 0;

	return true;
}

bool D3DText::CreateSentence(D3DSys* d3d, int id, int maxLength, char* text, int posX, int posY, float r, float g, float b)
{
	if (!InitSentence(id, maxLength, d3d->GetDevice()))
		return false;

	if (!UpdateSentence(id, text, posX, posY, r, g, b, d3d->GetDeviceContext()))
		return false;

	return true;
}

bool D3DText::UpdateSentence(int ID, char* text, int positionX, int positionY, float red, float green, float blue,
							   ID3D11DeviceContext* deviceContext)
{
	int numLetters;
	VertexType* vertices;
	float drawX, drawY;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* verticesPtr;

	// Store the color of the sentence.
	m_sentenceArray[ID].r = red;
	m_sentenceArray[ID].g = green;
	m_sentenceArray[ID].b = blue;

	// Get the number of letters in the sentence.
	numLetters = (int)strlen(text);

	// Check for possible buffer overflow.
	if(numLetters > m_sentenceArray[ID].maxLength)
		return false;

	// Create the vertex array.
	vertices = new VertexType[m_sentenceArray[ID].vertexCount];
	if(!vertices)
		return false;

	// Initialize vertex array to zeros at first.
	memset(vertices, 0, (sizeof(VertexType) * m_sentenceArray[ID].vertexCount));
	// Calculate the starting location to draw the sentence on the screen at.

	// Calculate the X and Y pixel position on the screen to start drawing to.
	drawX = (float)(((m_screenWidth / 2) * -1) + positionX);
	drawY = (float)((m_screenHeight / 2) - positionY);
	// Use the font class to build the vertex array from the sentence text and sentence draw location.
	m_Font->BuildVertexArray((void*)vertices, text, drawX, drawY);
	
	// Lock the vertex buffer so it can be written to.
	if(FAILED(deviceContext->Map(m_sentenceArray[ID].vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
		return false;

	// Get a pointer to the data in the vertex buffer.
	verticesPtr = (VertexType*)mappedResource.pData;

	// Copy the data into the vertex buffer.
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * m_sentenceArray[ID].vertexCount));

	// Unlock the vertex buffer.
	deviceContext->Unmap(m_sentenceArray[ID].vertexBuffer, 0);

	// Release the vertex array as it is no longer needed.
	delete [] vertices;
	vertices = 0;

	return true;
}

void D3DText::ReleaseSentence(int ID)
{
	if (m_sentenceArray[ID].vertexBuffer)
	{
		m_sentenceArray[ID].vertexBuffer->Release();
		m_sentenceArray[ID].vertexBuffer = 0;

	}

	if (m_sentenceArray[ID].indexBuffer)
	{
		m_sentenceArray[ID].indexBuffer->Release();
		m_sentenceArray[ID].indexBuffer = 0;
	}
	return;
}

bool D3DText::RenderSentence(ID3D11DeviceContext* deviceContext, int ID,  XMMATRIX orthoMatrix)
{
	unsigned int stride, offset;
	XMFLOAT4 pixelColour;

	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType); 
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_sentenceArray[ID].vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_sentenceArray[ID].indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Create a pixel colour vector with the input sentence colour.
	pixelColour = XMFLOAT4(m_sentenceArray[ID].r, m_sentenceArray[ID].g, m_sentenceArray[ID].b, 1.0f);

	// Render the text using the font shader.
	if(!m_FontShader->Render(deviceContext, m_sentenceArray[ID].indexCount, m_worldMatrix, m_baseViewMatrix, orthoMatrix, m_Font->GetTexture(), pixelColour))
		false;

	return true;
}


/*
bool D3DText::SetMousePosition(int mouseX, int mouseY, ID3D11DeviceContext* deviceContext)
{
	char tempString[16];
	char mouseString[16];
	bool result;

	// Convert the mouseX integer to string format.
	_itoa_s(mouseX, tempString, 10);

	// Setup the mouseX string.
	strcpy_s(mouseString, "Mouse X: ");
	strcat_s(mouseString, tempString);

	// Update the sentence vertex buffer with the new string information.
	result = UpdateSentence(m_sentence1, mouseString, 0, 0, 1.0f, 1.0f, 1.0f, deviceContext);
	if(!result)
	{
		return false;
	}

	// Convert the mouseY integer to string format.
	_itoa_s(mouseY, tempString, 10);

	// Setup the mouseY string.
	strcpy_s(mouseString, "Mouse Y: ");
	strcat_s(mouseString, tempString);

	// Update the sentence vertex buffer with the new string information.
	result = UpdateSentence(m_sentence2, mouseString, 0, 15, 1.0f, 1.0f, 1.0f, deviceContext);
	if(!result)
	{
		return false;
	}

	return true;
}

bool D3DText::SetFPS(int fps, ID3D11DeviceContext* deviceContext)
{
	char tempString[16];
	char fpsString[16];
	float red, green, blue;

	// Truncate the fps to below 10,000.
	if(fps > 9999)
		fps = 9999;

	// Convert the fps integer to string format.
	_itoa_s(fps, tempString, 10);

	// Setup the fps string.
	strcpy_s(fpsString, "Fps: ");
	strcat_s(fpsString, tempString);

	// If fps is 60 or above set the fps color to green.
	if(fps >= 60)
	{
		red = 0.0f;
		green = 1.0f;
		blue = 0.0f;
	}

	// If fps is below 60 set the fps color to yellow.
	if(fps < 60)
	{
		red = 1.0f;
		green = 1.0f;
		blue = 0.0f;
	}

	// If fps is below 30 set the fps color to red.
	if(fps < 30)
	{
		red = 1.0f;
		green = 0.0f;
		blue = 0.0f;
	}

	// Update the sentence vertex buffer with the new string information.
	if(!UpdateSentence(FPSText, fpsString, 0, 50, 1.0f, 1.0f, 0.0f, deviceContext))
		return false;

	return true;
}

bool D3DText::SetCPU(int cpu, ID3D11DeviceContext* deviceContext)
{
	char tempString[16];
	char cpuString[16];

	// Convert the cpu integer to string format.
	_itoa_s(cpu, tempString, 10);

	// Setup the cpu string.
	strcpy_s(cpuString, "Cpu: ");
	strcat_s(cpuString, tempString);
	strcat_s(cpuString, "%");

	// Update the sentence vertex buffer with the new string information.
	if(!UpdateSentence(CPUText, cpuString, 0, 65, 1.0f, 1.0f, 0.0f, deviceContext))
		return false;

	return true;
}
*/