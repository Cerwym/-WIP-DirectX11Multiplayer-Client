#ifndef D3DMODEL_H
#define	D3DMODEL_H

#include <d3d11.h>
#include <D3DX10math.h>
#include <fstream>

#include "..\Defines.h"
#include "..\D3DShaderManager.h"
#include "D3DTexture.h"
#include "D3DLight.h"

using namespace std;

class D3DModel
{
private:

public:
	D3DModel();
	D3DModel( D3DXVECTOR3& pos );
	D3DModel(const D3DModel&){}
	~D3DModel();

	bool Init(ID3D11Device*, char*, WCHAR*);
	// void Render(ID3D11DeviceContext*);
	void Render( ID3D11DeviceContext* deviceContext, D3DShaderManager* sMgr, D3DXMATRIX view, D3DXMATRIX proj );
	void Render( ID3D11DeviceContext* deviceContext, D3DShaderManager* sMgr, D3DXMATRIX view, D3DXMATRIX proj, D3DXVECTOR3 cameraPos, D3DLight* lightObj );

	bool LoadTexture(ID3D11Device*, WCHAR*);
	bool LoadModel(char*);

	int GetIndexCount();
	D3DXVECTOR3 GetPosition() { return m_Position; }
	D3DXMATRIX GetWorld() { return m_WorldMatrix; }
	void TranslateBy( float x, float y, float z );
	void TranslateTo( float x, float y, float z );
	void RotateBy( float x, float y, float z );
	ID3D11ShaderResourceView* GetTexture(){return m_Texture->GetTexture();}

private:

	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
		D3DXVECTOR3 normal;
	};

	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

	bool InitBuffers(ID3D11Device*);
	void RebuildTransform();

	ID3D11Buffer *m_VertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;
	D3DTexture* m_Texture;
	ModelType* m_Model;

	// The object's position in local space
	D3DXVECTOR3 m_Position;
	// The object's scale
	D3DXVECTOR3 m_Scale;
	// The object's rotation data
	D3DXVECTOR3 m_Rotation;

	// The object's local world matrix;
	D3DXMATRIX m_WorldMatrix;
};

#endif