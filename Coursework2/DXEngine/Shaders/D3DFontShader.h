#pragma once

#include "..\Defines.h"
#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dx11async.h>
#include <fstream>
#include <sstream>
#include <io.h>

using namespace std;

class D3DFontShader
{
public:
	
	D3DFontShader();
	D3DFontShader(const D3DFontShader&){}
	~D3DFontShader();

	bool Init(ID3D11Device*, HWND);
	bool Render(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, XMFLOAT4);

private:

	struct ConstantBufferType 
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

	struct PixelBufferType
	{
		XMFLOAT4 pixelColour;
	};

	bool InitShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, XMFLOAT4);
	void RenderShader(ID3D11DeviceContext*, int);

	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_constantBuffer;
	ID3D11SamplerState* m_sampleState;

	ID3D11Buffer* m_pixelBuffer;
};

