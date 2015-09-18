#pragma once

#include "..\Defines.h"
#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dx11async.h>
#include <fstream>
#include <sstream>
#include <io.h>
using namespace std;

class D3DLightShader
{
public:
	D3DLightShader();
	D3DLightShader(const D3DLightShader&){}
	~D3DLightShader();

	bool Init(ID3D11Device*, HWND);
	bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, XMFLOAT3, D3DXVECTOR4, D3DXVECTOR4, 
		XMFLOAT3, D3DXVECTOR4, float);

private:
	bool InitShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);
	
	bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, XMFLOAT3, D3DXVECTOR4, D3DXVECTOR4,
		XMFLOAT3, D3DXVECTOR4, float);
	void RenderShader(ID3D11DeviceContext*, int);

	struct MatrixBufferType 
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
	};

	struct CameraBufferType
	{
		XMFLOAT3 cameraPosition;
		float padding; // Added extra bits for struct is a multiple of 16 for CreateBuffer() requirements.
	};

	struct LightBufferType
	{
		D3DXVECTOR4 ambientColour;
		D3DXVECTOR4 diffuseColour;
		XMFLOAT3 lightDirection;
		float specularPower;
		D3DXVECTOR4 specularColour;
	};

	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11SamplerState* m_sampleState;
	ID3D11Buffer* m_matrixBuffer;

	ID3D11Buffer* m_cameraBuffer;
	ID3D11Buffer* m_lightBuffer;
};

