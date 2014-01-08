#include "D3DTexture.h"

D3DTexture::D3DTexture()
{
	m_Texture = 0;
}

D3DTexture::D3DTexture(ID3D11ShaderResourceView* resource )
{
	m_Texture = resource;
}

D3DTexture::~D3DTexture()
{
	S_RELEASE(m_Texture);
}

bool D3DTexture::Init(ID3D11Device* device, WCHAR* filename)
{
	HRESULT result = D3DX11CreateShaderResourceViewFromFile(device, filename, NULL, NULL, &m_Texture, NULL);
	if (FAILED(result))
		return false;
	return true;
}