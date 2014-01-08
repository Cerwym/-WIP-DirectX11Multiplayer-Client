#ifndef D3DTEXT_H
#define D3DTEXT_H

#include "Defines.h"
#include "Objects/D3DFont.h"
#include "Shaders/D3DFontShader.h"

class D3DText
{
public:

	D3DText();
	D3DText(const D3DText&){}
	~D3DText();

	bool Init(ID3D11Device*, ID3D11DeviceContext*, HWND, int, int, D3DXMATRIX);
	bool Render(ID3D11DeviceContext*, D3DXMATRIX);

	bool SetMousePosition(int, int, ID3D11DeviceContext*);
	bool SetFPS(int, ID3D11DeviceContext*);
	bool SetCPU(int, ID3D11DeviceContext*);
	
private:

	struct SentanceType
	{
		ID3D11Buffer *vertexBuffer, *indexBuffer;
		int vertexCount, indexCount, maxLength;
		float r,g,b;
	};

	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
	};

	bool InitShader(SentanceType**, int, ID3D11Device*);
	bool InitSentence(SentanceType**, int, ID3D11Device*);
	bool UpdateSentence(SentanceType*, char*, int, int, float, float, float, ID3D11DeviceContext*);
	void ReleaseSentence(SentanceType**);
	bool RenderSentence(ID3D11DeviceContext*, SentanceType*, D3DXMATRIX);

	D3DFont* m_Font;
	D3DFontShader* m_FontShader;
	int m_screenWidth, m_screenHeight;
	D3DXMATRIX m_baseViewMatrix;
	D3DXMATRIX m_worldMatrix;

	SentanceType* m_sentence1;
	SentanceType* m_sentence2;

	SentanceType* CPUText;
	SentanceType* FPSText;
};

#endif