#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <assert.h>

#include "..\Defines.h"
#include "..\D3DShaderManager.h"
#include "D3DTexture.h"

class D3DBitmap
{
	struct Vector2I 
	{
		int x, y;
	};
public:
	D3DBitmap();
	D3DBitmap(const D3DBitmap&){}
	~D3DBitmap();

	/**
    Init()

    @param device - The Virtual Adapter to use
	@param screenWidth, screenHeight - the dimensions of the screen, used to determine how far left from origin the bitmap is
    @param bitmapFname - The filepath of where the texture is to be loaded from (NULLABLE)
	@param bitmapWidth, bitmapHeight - the size dimensions of the texture
	@return success of initialization
	*/
	bool Init(ID3D11Device* device, int screenWidth, int screenHeight, WCHAR* bitmapFName, int bitmapWidth, int bitmapHeight);
	bool Render( ID3D11DeviceContext* deviceContext, D3DShaderManager* sm, D3DXMATRIX view, D3DXMATRIX proj );

	int GetIndexCount(){return m_indexCount;}
	int GetWidth(){return m_bitmapWidth;}
	int GetHeight(){return m_bitmapHeight;}
	Vector2I GetPosition() { return m_Position;}
	void SetPosition(int x, int y) { m_Position.x = x; m_Position.y = y; }
	void MoveBy(int x, int y) { m_Position.x +=x; m_Position.y += y; }
	ID3D11ShaderResourceView* GetTexture(){return m_Texture->GetTexture();}
	void SetTexture( ID3D11ShaderResourceView* texture);

private:
	struct VertexType
	{
		XMFLOAT3 position;
		D3DXVECTOR2 texture;
	};

	bool InitBuffers(ID3D11Device*);
	bool UpdateBuffers(ID3D11DeviceContext*, int, int);
	void RenderBuffers(ID3D11DeviceContext*);
	bool LoadTexture(ID3D11Device*, WCHAR*);

	ID3D11Buffer *m_vertexBuffer, *m_IndexBuffer;
	D3DXMATRIX m_worldMatrix;
	D3DXMATRIX m_baseViewMatrix;
	int m_vertexCount, m_indexCount;
	D3DTexture* m_Texture;
	Vector2I m_Position;
	int m_screenWidth, m_screenHeight;
	int m_bitmapWidth, m_bitmapHeight;
	int m_previousPosX, m_previousPosY;
};