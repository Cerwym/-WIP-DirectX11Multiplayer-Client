#include "D3DShaderManager.h"


D3DShaderManager::D3DShaderManager()
{
	m_TextureShader = 0;
	m_D3DLightShader = 0;
	m_BumpMapShader = 0;
}


D3DShaderManager::D3DShaderManager(const D3DShaderManager& other)
{
}

D3DShaderManager::~D3DShaderManager()
{
	// Safely delete all the shader objects.
	S_DELETE(m_TextureShader);
	S_DELETE(m_D3DLightShader);
	S_DELETE(m_BumpMapShader);
}


bool D3DShaderManager::Init(ID3D11Device* device, HWND hwnd)
{
	// Create the texture shader object.
	m_TextureShader = new D3DTextureShader;
	if(!m_TextureShader)
		return false;

	// Initialize the texture shader object.
	if(!m_TextureShader->Init(device, hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the light shader object.
	m_D3DLightShader = new D3DLightShader;
	if(!m_D3DLightShader)
		return false;

	// Initialize the light shader object.
	if(!m_D3DLightShader->Init(device, hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the bump map shader object.
	m_BumpMapShader = new D3DBumpMapShader;
	if(!m_BumpMapShader)
		return false;

	// Initialize the bump map shader object.
	if(!m_BumpMapShader->Init(device, hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the bump map shader object.", L"Error", MB_OK);
		return false;
	}

	return true;
}

bool D3DShaderManager::RenderTextureShader( ID3D11DeviceContext* device, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture )
{
	// Render the model using the texture shader.
	if(!m_TextureShader->Render(device, indexCount, worldMatrix, viewMatrix, projectionMatrix, texture))
		return false;

	return true;
}


bool D3DShaderManager::RenderLightShader(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, 
										   ID3D11ShaderResourceView* texture, D3DXVECTOR3 &camPosition, D3DLight* lightObject)
{
	// Render the model using the light shader.
	if(! m_D3DLightShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, texture, lightObject->GetDirection(), lightObject->GetAmbientColour(), lightObject->GetDiffuseColour(), camPosition, 
		lightObject->GetSpecularColour(), lightObject->GetSpecularPower()))
		return false;

	return true;
}


bool D3DShaderManager::RenderBumpMapShader(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, 
											 ID3D11ShaderResourceView* colorTexture, ID3D11ShaderResourceView* normalTexture, D3DLight* lightObject)
{
	if(!m_BumpMapShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, colorTexture, normalTexture, lightObject->GetDirection(), lightObject->GetDiffuseColour()))
		return false;

	return true;
}