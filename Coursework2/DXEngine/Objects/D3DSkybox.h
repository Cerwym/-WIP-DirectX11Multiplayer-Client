#ifndef D3DSKYBOX_H 
#define D3DSKYBOX_H

#include <d3d11.h>
#include <D3DX10math.h>
#include <D3DX11async.h> // To compile the shader
#include <vector>
#include "..\Defines.h"
#include "D3DCamera.h"
#include <fstream>
#include <sstream>
#include <io.h>
using namespace std;
class D3DSkyBox
{

public:

	D3DSkyBox();
	~D3DSkyBox();

	bool Init(ID3D11Device* d3d11Device, int LatLines, int LongLines, WCHAR* skyTexture, HWND hwnd);
	void Draw(ID3D11DeviceContext* deviceContext, D3DCamera* camera);
	
private:

	// Vertex Structure
	struct Vertex
	{
		Vertex(){}
		Vertex(float x, float y, float z, float u, float v, float nx, float ny, float nz) : Position(x,y,z), TextureC(u, v), Normals(nx, ny, nz){}

		D3DXVECTOR3 Position;
		D3DXVECTOR2 TextureC;
		D3DXVECTOR3 Normals;
	};

	struct ConstantBuffer
	{
		D3DXMATRIX WVP;
		D3DXMATRIX World;
	}; ConstantBuffer m_CB;

	bool LoadTexture(ID3D11Device* d3d11Device, WCHAR* skyTexture, HWND hwnd);
	void OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename);

	ID3D11Buffer* m_IndexBuffer;
	ID3D11Buffer* m_VertexBuffer;
	ID3D11VertexShader* SKYBOX_VS;
	ID3D11PixelShader* SKYBOX_PS;
	ID3D11Buffer* m_MatrixBuffer;
	ID3D10Blob* SKYBOX_VS_Buffer;
	ID3D10Blob* SKYBOX_PS_Buffer;

	// Change this to D3DTexture
	ID3D11ShaderResourceView* m_Texture;
	ID3D11SamplerState* m_sampleState;

	int m_Vertices, m_Faces;

	D3DXMATRIX m_WorldMatrix;
	D3DXMATRIX m_ScaleMatrix;
	D3DXMATRIX m_TranslationMatrix;

	D3DXMATRIX m_RotationX;
	D3DXMATRIX m_RotationY;
	D3DXMATRIX m_RotationZ;
};

#endif