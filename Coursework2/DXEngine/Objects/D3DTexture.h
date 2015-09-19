#pragma once

#include <d3d11.h>
#include <D3DX11tex.h>

#include "..\Defines.h"

class D3DTexture
{
public:
	D3DTexture();
	D3DTexture(ID3D11ShaderResourceView* );
	D3DTexture(const D3DTexture&){}
	~D3DTexture();

	bool Init(ID3D11Device* device, WCHAR* filename);

	ID3D11ShaderResourceView* GetTexture(){return m_Texture;}

private:
	ID3D11ShaderResourceView* m_Texture;
};

