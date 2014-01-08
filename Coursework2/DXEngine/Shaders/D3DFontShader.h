#ifndef D3DFONTSHADER_H
#define D3DFONTSHADER_H

#include "..\Defines.h"
#include <d3d11.h>
#include <d3dx10math.h>
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
	bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, D3DXVECTOR4);

private:

	struct ConstantBufferType 
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
	};

	struct PixelBufferType
	{
		D3DXVECTOR4 pixelColour;
	};

	bool InitShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, D3DXVECTOR4);
	void RenderShader(ID3D11DeviceContext*, int);

	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_constantBuffer;
	ID3D11SamplerState* m_sampleState;

	ID3D11Buffer* m_pixelBuffer;
};

#endif