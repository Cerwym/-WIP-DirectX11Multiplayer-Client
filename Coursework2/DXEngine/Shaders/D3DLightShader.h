#pragma once

#include "..\Defines.h"
#include <d3d11.h>
#include <DirectXMath.h>
#include <fstream>
#include <sstream>
#include <io.h>
using namespace std;
using namespace DirectX;

class D3DLightShader
{
public:
	D3DLightShader();
	D3DLightShader(const D3DLightShader&){}
	~D3DLightShader();

	bool Init(ID3D11Device*, HWND);
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, const XMMATRIX &worldMatrix, const XMMATRIX &viewMatrix,
		const XMMATRIX &projectionMatrix, ID3D11ShaderResourceView* texture, XMFLOAT3 lightDirection, XMFLOAT4 ambientColour, XMFLOAT4 diffuseColour, XMFLOAT3 cameraPosition, XMFLOAT4 specularColour, float specularPower);

private:
	bool InitShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);
	
	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &worldMatrix, const XMMATRIX &viewMatrix,
		const XMMATRIX &projectionMatrix, ID3D11ShaderResourceView* texture, XMFLOAT3 lightDirection,
		XMFLOAT4 ambientColour, XMFLOAT4 diffuseColor, XMFLOAT3 cameraPosition, XMFLOAT4 specularColour, float specularPower);

	void RenderShader(ID3D11DeviceContext*, int);

	struct MatrixBufferType 
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

	struct CameraBufferType
	{
		XMFLOAT3 cameraPosition;
		float padding; // Added extra bits for struct is a multiple of 16 for CreateBuffer() requirements.
	};

	struct LightBufferType
	{
		XMFLOAT4 ambientColour;
		XMFLOAT4 diffuseColour;
		XMFLOAT3 lightDirection;
		float specularPower;
		XMFLOAT4 specularColour;
	};

	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11SamplerState* m_sampleState;
	ID3D11Buffer* m_matrixBuffer;

	ID3D11Buffer* m_cameraBuffer;
	ID3D11Buffer* m_lightBuffer;
};

