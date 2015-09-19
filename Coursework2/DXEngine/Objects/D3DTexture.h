#pragma once

#include <d3d11.h>
#include <stdio.h>

#include "..\Defines.h"

class D3DTexture
{
public:
	D3DTexture();
	D3DTexture(ID3D11ShaderResourceView* );
	D3DTexture(const D3DTexture&){}
	~D3DTexture();

	bool Init(ID3D11Device* device, ID3D11DeviceContext* context, char* filename);

	ID3D11ShaderResourceView* GetTexture(){ return m_TextureView; }

private:

	struct TGAHeader
	{
		unsigned char data[12];
		unsigned short width;
		unsigned short height;
		unsigned char bpp;
		unsigned char data2;
	};

	bool LoadTGA(char*, int&, int &);

	unsigned char* m_TGAImage;
	ID3D11Texture2D* m_Texture;
	ID3D11ShaderResourceView* m_TextureView;
};

