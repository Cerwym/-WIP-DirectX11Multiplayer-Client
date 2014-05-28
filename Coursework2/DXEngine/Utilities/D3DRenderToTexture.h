#pragma once

#include "..\Defines.h"
#include <d3d11.h>

class D3DRenderToTexture
{
public:
	D3DRenderToTexture();
	D3DRenderToTexture(const D3DRenderToTexture&){}
	~D3DRenderToTexture();

	bool Initialize( ID3D11Device* device, int textureWidth, int textureHeight );
	void SetRenderTarget( ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView );
	void ClearRenderTarget( ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView, float red, float green, float blue, float alpha );

	ID3D11ShaderResourceView* GetShaderResourceView();

private:
	ID3D11Texture2D* m_renderTargetTexture;
	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11ShaderResourceView* m_shaderResourceView;
};

