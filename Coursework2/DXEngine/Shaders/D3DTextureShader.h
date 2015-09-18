#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
//#include <D3DX11async.h>
#include <fstream>
#include <sstream>
#include <io.h>

#include "..\Defines.h"

using namespace std;
using namespace DirectX;

class D3DTextureShader
{
public:
	D3DTextureShader();
	D3DTextureShader(const D3DTextureShader&){}
	~D3DTextureShader();

	bool Init(ID3D11Device*, HWND);
	bool Render(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*);

private:
	bool InitShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*);
	void RenderShader(ID3D11DeviceContext*, int);

	struct MatrixBufferType // Naming convention to match the shader file
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_Layout;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11SamplerState* m_sampleState;
};

