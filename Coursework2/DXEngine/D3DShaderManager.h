#pragma once

#include "D3DSys.h"
#include "Defines.h"
#include "Shaders/D3DTextureShader.h"
#include "Shaders/D3DLightShader.h"
#include "Shaders/D3DBumpmapShader.h"
#include "Objects/D3DLight.h"

class D3DShaderManager
{
public:
	D3DShaderManager();
	D3DShaderManager(const D3DShaderManager&);
	~D3DShaderManager();

	bool Init(ID3D11Device*, HWND);

	bool RenderTextureShader( ID3D11DeviceContext* device, int indexCount, XMMATRIX &worldMatrix, XMMATRIX &viewMatrix, XMMATRIX &projectionMatrix, ID3D11ShaderResourceView* texture );

	bool RenderLightShader( ID3D11DeviceContext* deviceContext, int indexCount, const XMMATRIX &worldMatrix, const XMMATRIX &viewMatrix, const XMMATRIX &projectionMatrix, 
		ID3D11ShaderResourceView* texture, XMFLOAT3 &camPosition, D3DLight* lightObject );

	bool RenderBumpMapShader( ID3D11DeviceContext* deviceContext, int indexCount, const XMMATRIX &worldMatrix, const XMMATRIX &viewMatrix, const XMMATRIX &projectionMatrix, 
		ID3D11ShaderResourceView* colorTexture, ID3D11ShaderResourceView* normalTexture, D3DLight* lightObject );

private:
	D3DTextureShader* m_TextureShader;
	D3DLightShader* m_D3DLightShader;
	D3DBumpMapShader* m_BumpMapShader;
};

