#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <fstream>

#include "..\Defines.h"
#include "..\D3DShaderManager.h"
#include "D3DCamera.h"
#include "D3DTexture.h"
#include "D3DLight.h"

using namespace std;

class D3DModel
{
private:
	enum CullMode
	{
		CULL_NEVER, CULL_SPHERE, CULL_CUBE, CULL_POINT 
	};
public:
	D3DModel();
	D3DModel( XMFLOAT3& pos );
	D3DModel(const D3DModel&){}
	~D3DModel();

	// Initialize the Model with the D3DDevice, Model File, Texture File ( NULLABLE ), Bump Map ( NULLABLE )
	bool Init( ID3D11Device* device, char* modelFName, WCHAR* textureFName, WCHAR* bumpMapFname = NULL);

	bool Render( ID3D11DeviceContext* deviceContext, D3DShaderManager* sMgr, D3DCamera* camera, D3DLight* lightObj, const int cullMode = CULL_CUBE );

	XMFLOAT3 GetPosition() { return m_Position; }
	XMMATRIX GetWorld() { return m_WorldMatrix; }

	void NO_GSBillboard( D3DCamera* camera);

	void TranslateBy( float x, float y, float z );
	void TranslateTo( float x, float y, float z );
	void RotateBy( float x, float y, float z );
	void SetRotation( float x, float y, float z);
	ID3D11ShaderResourceView* GetTexture(){return m_Texture->GetTexture();}

private:
	
	// Vertex Structure for an object using texture mapping
	struct TexVertex
	{
		XMFLOAT3 position;
		D3DXVECTOR2 texture;
		XMFLOAT3 normal;
	};

	// Vertex Structure for an object using bump mapping
	struct BumpVertex
	{
		XMFLOAT3 position;
		D3DXVECTOR2 texture;
		XMFLOAT3 normal;
		XMFLOAT3 tangent;
		XMFLOAT3 binormal;
	};

	// Model structure for an object using texture mapping
	struct TexModel
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

	// Model structure for an object using bump mapping
	struct BumpModel
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
		float tx, ty, tz;
		float bx, by, bz;
	};          

	struct Vector3F
	{
		float x, y, z;
	};

	struct InstancedData
	{
		XMFLOAT3 position;
	};

	bool InitBuffers(ID3D11Device*);
	void RebuildTransform();

	bool LoadTexture(ID3D11Device*, WCHAR*, WCHAR*);
	bool LoadModel(char*, bool);

	void CalculateModelVectors();
	void CalculateTangentBinormal(TexModel, TexModel, TexModel, Vector3F&, Vector3F&);

	ID3D11Buffer *m_VertexBuffer, *m_indexBuffer, *m_instanceBuffer;
	int m_vertexCount, m_indexCount, m_instanceCount;
	D3DTexture* m_Texture;

	// bump map normal texture
	D3DTexture* m_NormalMapTexture;
	// Texture data if not using bump mapping
	TexModel* m_Model;
	// Texture data if using bump mapping
	BumpModel* m_BModel;

	// The object's position in local space
	XMFLOAT3 m_Position;
	// The object's scale
	XMFLOAT3 m_Scale;
	// The object's rotation data
	XMFLOAT3 m_Rotation;
	// The object's local world matrix;
	XMMATRIX m_WorldMatrix;

	bool m_isBumpMapped;
};

