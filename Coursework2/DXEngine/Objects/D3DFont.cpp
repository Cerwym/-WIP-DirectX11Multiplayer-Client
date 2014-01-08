#include "D3DFont.h"

D3DFont::D3DFont()
{
	m_Font = 0;
	m_Texture = 0;
}

D3DFont::~D3DFont()
{
	if (m_Texture)
	{
		delete m_Texture;
		m_Texture = 0;
	}

	if (m_Font)
	{
		delete [] m_Font;
		m_Font = 0;
	}
}

bool D3DFont::Init(ID3D11Device* device, char* fontFName, WCHAR* textureFName)
{
	if (!LoadFontData(fontFName))
		return false;
	if (!LoadTexture(device, textureFName))
		return false;
	return true;
}

bool D3DFont::LoadFontData(char* filename)
{
	ifstream file;
	char temp;

	m_Font = new FontType[95]; // The font contains 95 usable ASCII characters
	if (!m_Font)
		return false;

	// Read in the font size and spacing between characters
	file.open(filename);
	if (file.fail() || file.bad())
		return false;

	for (int i = 0; i < 95; i++)
	{
		file.get(temp);
		while (temp != ' ')
			file.get(temp);
		file.get(temp);
		while(temp != ' ')
			file.get(temp);

		file >> m_Font[i].left;
		file >> m_Font[i].right;
		file >> m_Font[i].size;
	}

	file.close();

	return true;
}

bool D3DFont::LoadTexture(ID3D11Device* device, WCHAR* filename)
{
	m_Texture = new D3DTexture;
	if (!m_Texture)
		return false;

	if (!m_Texture->Init(device, filename))
		return false;

	return true;
}

void D3DFont::BuildVertexArray(void* verts, char* sentance, float drawX, float drawY)
{
	VertexType* vertexPtr;
	int numLetters, index, letter;

	// Because the type can't be copied (void*) typecast it into a VertexType structure
	vertexPtr = (VertexType*)verts;

	numLetters = (int)strlen(sentance);

	index = 0;

	for (int i = 0; i < numLetters; i++)
	{
		letter = ((int)sentance[i]) -32;

		// If the letter is a space, then create a physical gap of 3 pixels on screen
		if (letter == 0)
			drawX = drawX + 3.0f;
		else
		{
			// First triangle in quad.
			vertexPtr[index].position = D3DXVECTOR3(drawX, drawY, 0.0f);  // Top left.
			vertexPtr[index].texture = D3DXVECTOR2(m_Font[letter].left, 0.0f);
			index++;

			vertexPtr[index].position = D3DXVECTOR3((drawX + m_Font[letter].size), (drawY - 16), 0.0f);  // Bottom right.
			vertexPtr[index].texture = D3DXVECTOR2(m_Font[letter].right, 1.0f);
			index++;

			vertexPtr[index].position = D3DXVECTOR3(drawX, (drawY - 16), 0.0f);  // Bottom left.
			vertexPtr[index].texture = D3DXVECTOR2(m_Font[letter].left, 1.0f);
			index++;

			// Second triangle in quad.
			vertexPtr[index].position = D3DXVECTOR3(drawX, drawY, 0.0f);  // Top left.
			vertexPtr[index].texture = D3DXVECTOR2(m_Font[letter].left, 0.0f);
			index++;

			vertexPtr[index].position = D3DXVECTOR3(drawX + m_Font[letter].size, drawY, 0.0f);  // Top right.
			vertexPtr[index].texture = D3DXVECTOR2(m_Font[letter].right, 0.0f);
			index++;

			vertexPtr[index].position = D3DXVECTOR3((drawX + m_Font[letter].size), (drawY - 16), 0.0f);  // Bottom right.
			vertexPtr[index].texture = D3DXVECTOR2(m_Font[letter].right, 1.0f);
			index++;

			// Update the x location for drawing by the size of the letter and one pixel.
			drawX = drawX + m_Font[letter].size + 1.0f;
		}
	}

	return;
}