#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <fstream>
#include "D3DTexture.h"

#include <memory>
using std::unique_ptr;

using namespace std;
using namespace DirectX;

class D3DFont
{
public:

	D3DFont();
	D3DFont(const D3DFont&){}
	~D3DFont();

	bool Init(ID3D11Device* device, ID3D11DeviceContext* context, char* fontFName, char* textureFName);

	ID3D11ShaderResourceView* GetTexture(){return m_Texture->GetTexture();}
	void BuildVertexArray(void*, char*, float, float);

private:

	struct FontType
	{
		float left, right;
		int size;
	};

	struct VertexType 
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};

	bool LoadFontData(char*);
	void ReleaseFontData();
	bool LoadTexture(ID3D11Device* device, ID3D11DeviceContext*, char* filename);
	void ReleaseTexture();

	FontType* m_Font;
	D3DTexture* m_Texture;
};