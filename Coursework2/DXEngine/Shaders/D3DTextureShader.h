#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <fstream>
#include <sstream>
#include <io.h>
// Temporary
#include <d3dcompiler.h>
#include <d3dx11effect.h>
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
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, const XMMATRIX &worldMatrix, const XMMATRIX &viewMatrix,
		const XMMATRIX &projectionMatrix, ID3D11ShaderResourceView* texture);

private:
	bool InitShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &worldMatrix, const XMMATRIX &viewMatrix,
		const XMMATRIX &projectionMatrix, ID3D11ShaderResourceView* texture);
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

