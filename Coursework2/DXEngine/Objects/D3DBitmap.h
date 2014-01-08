#ifndef D3DBITMAP_H
#define D3DBITMAP_H

#include <d3d11.h>
#include <D3DX10math.h>

#include "..\Defines.h"
#include "D3DTexture.h"

class D3DBitmap
{
public:
	D3DBitmap();
	D3DBitmap(const D3DBitmap&){}
	~D3DBitmap();

	bool Init(ID3D11Device*, int, int, WCHAR*, int, int);
	bool Render(ID3D11DeviceContext*, int, int);

	int GetIndexCount(){return m_indexCount;}
	int GetWidth(){return m_bitmapWidth;}
	int GetHeight(){return m_bitmapHeight;}
	ID3D11ShaderResourceView* GetTexture(){return m_Texture->GetTexture();}

private:
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
	};

	bool InitBuffers(ID3D11Device*);
	bool UpdateBuffers(ID3D11DeviceContext*, int, int);
	void RenderBuffers(ID3D11DeviceContext*);
	bool LoadTexture(ID3D11Device*, WCHAR*);

	ID3D11Buffer *m_vertexBuffer, *m_IndexBuffer;
	int m_vertexCount, m_indexCount;
	D3DTexture* m_Texture;

	int m_screenWidth, m_screenHeight;
	int m_bitmapWidth, m_bitmapHeight;
	int m_previousPosX, m_previousPosY;
};

#endif