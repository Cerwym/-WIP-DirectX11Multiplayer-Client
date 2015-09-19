#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

#include "D3DTexture.h"
#include "D3DCamera.h"

class D3DParticleEmitter
{
private:
	struct ParticleType
	{
		float positionX, positionY, positionZ;
		float velocity;
		bool active;
	};

	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT4 color;
	};

public:
	D3DParticleEmitter();
	D3DParticleEmitter(const D3DParticleEmitter&);
	~D3DParticleEmitter();

	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* context, char* textureFilename);
	bool Frame(float, ID3D11DeviceContext*, D3DCamera* cam );
	void Render(ID3D11DeviceContext*);

	XMFLOAT3 GetPosition() { return m_Position; }
	XMMATRIX GetWorld(){ return m_WorldMatrix; }

	void TranslateTo( float x, float y, float z );
	void TranslateTo( XMFLOAT3& pos );
	void RotateBy( float x, float y, float z );
	void SetRotation( float x, float y, float z);
	void NO_GSBillboard( D3DCamera* camera);

	ID3D11ShaderResourceView* GetTexture();
	int GetIndexCount();

private:
	bool LoadTexture(ID3D11Device* device, ID3D11DeviceContext* context, char* filename);
	void ReleaseTexture();

	bool InitializeParticleSystem();
	void ShutdownParticleSystem();

	bool InitializeBuffers(ID3D11Device*);
	void RebuildTransform();
	void ShutdownBuffers();

	void EmitParticles(float);
	void UpdateParticles(float);
	void KillParticles();

	bool UpdateBuffers(ID3D11DeviceContext*);
	void RenderBuffers(ID3D11DeviceContext*);

private:
	float m_particleDeviationX, m_particleDeviationY, m_particleDeviationZ;
	float m_particleVelocity, m_particleVelocityVariation;
	float m_particleSize, m_particlesPerSecond;
	int m_maxParticles;

	int m_currentParticleCount;
	float m_accumulatedTime;

	D3DTexture* m_Texture;
	ParticleType* m_particleList;
	int m_vertexCount, m_indexCount;
	VertexType* m_vertices;
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;

	// This instance of the particle emitter's local world matrix
	XMMATRIX m_WorldMatrix;

	// This instance of the particle emitter's local rotation data
	XMFLOAT3 m_Rotation;
	XMFLOAT3 m_Position;
	XMFLOAT3 m_Scale;
};

