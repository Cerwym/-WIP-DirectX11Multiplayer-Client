#ifndef D3DBUMPMAPSHADER_H
#define D3DBUMPMAPSHADER_H

#include "..\Defines.h"
#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <fstream>
#include <sstream>
#include <io.h>

using namespace std;

class D3DBumpMapShader
{
public:
	D3DBumpMapShader();
	D3DBumpMapShader(const D3DBumpMapShader&);
	~D3DBumpMapShader();

	bool Init(ID3D11Device*, HWND);
	bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, D3DXVECTOR3, D3DXVECTOR4);

private:
	bool InitShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);

	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, D3DXVECTOR3, D3DXVECTOR4);
	void RenderShader(ID3D11DeviceContext*, int);

	struct MatrixBufferType
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
	};

	struct LightBufferType
	{
		D3DXVECTOR4 diffuseColor;
		D3DXVECTOR3 lightDirection;
		float padding;
	};

	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11SamplerState* m_sampleState;
	ID3D11Buffer* m_lightBuffer;
};

#endif