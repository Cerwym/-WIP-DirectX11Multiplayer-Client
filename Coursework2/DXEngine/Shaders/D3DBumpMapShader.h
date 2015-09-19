#pragma once

#include "..\Defines.h"
#include <d3d11.h>
#include <DirectXMath.h>
#include <fstream>
#include <sstream>
#include <io.h>

using namespace std;
using namespace DirectX;

class D3DBumpMapShader
{
public:
	D3DBumpMapShader();
	D3DBumpMapShader(const D3DBumpMapShader&);
	~D3DBumpMapShader();

	bool Init(ID3D11Device*, HWND);
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, const XMMATRIX &worldMatrix, const XMMATRIX &viewMatrix,
		const XMMATRIX &projectionMatrix, ID3D11ShaderResourceView* colorTexture, ID3D11ShaderResourceView* normalMapTexture,
		XMFLOAT3 lightDirection, XMFLOAT4 diffuseColor);

private:
	bool InitShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);

	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &worldMatrix,
		const XMMATRIX &viewMatrix, const XMMATRIX &projectionMatrix,
		ID3D11ShaderResourceView* colorTexture, ID3D11ShaderResourceView* normalMapTexture,
		XMFLOAT3 lightDirection, XMFLOAT4 diffuseColor);

	void RenderShader(ID3D11DeviceContext*, int);

	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

	struct LightBufferType
	{
		XMFLOAT4 diffuseColor;
		XMFLOAT3 lightDirection;
		float padding;
	};

	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11SamplerState* m_sampleState;
	ID3D11Buffer* m_lightBuffer;
};

