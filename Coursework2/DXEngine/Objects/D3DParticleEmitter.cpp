#include "D3DParticleEmitter.h"


D3DParticleEmitter::D3DParticleEmitter()
{
	m_Texture = 0;
	m_particleList = 0;
	m_vertices = 0;
	m_vertexBuffer = 0;
	m_indexBuffer = 0;

	m_Scale = XMFLOAT3( 1.0f, 1.0f, 1.0f );
	m_Position = XMFLOAT3( 0.0f, 0.0f, 0.0f );
	m_Rotation = XMFLOAT3( 0.0f, 0.0f, 0.0f );
}


D3DParticleEmitter::D3DParticleEmitter(const D3DParticleEmitter& other)
{
}


D3DParticleEmitter::~D3DParticleEmitter()
{
	// Release the buffers.
	ShutdownBuffers();

	// Release the particle system.
	ShutdownParticleSystem();

	// Release the texture used for the particles.
	ReleaseTexture();
}


bool D3DParticleEmitter::Initialize(ID3D11Device* device, WCHAR* textureFilename)
{
	bool result;


	// Load the texture that is used for the particles.
	result = LoadTexture(device, textureFilename);
	if(!result)
	{
		return false;
	}

	// Initialize the particle system.
	result = InitializeParticleSystem();
	if(!result)
	{
		return false;
	}

	// Create the buffers that will be used to render the particles with.
	result = InitializeBuffers(device);
	if(!result)
	{
		return false;
	}

	RebuildTransform();

	return true;
}



void D3DParticleEmitter::NO_GSBillboard(D3DCamera* camera)
{
	double angle = atan2( m_Position.x - camera->GetPosition().x, m_Position.z - camera->GetPosition().z) * ( 180 / D3DX_PI );
	float rotation = angle * 0.0174532925f;
	SetRotation( 0, rotation, 0 );
}

void D3DParticleEmitter::TranslateTo( float x, float y, float z )
{
	m_Position = XMFLOAT3( x, y, z );
	RebuildTransform();
}

void D3DParticleEmitter::TranslateTo( XMFLOAT3& pos )
{
	m_Position = pos;
	RebuildTransform();
}

void D3DParticleEmitter::RotateBy( float x, float y, float z )
{
	m_Rotation += XMFLOAT3( x, y, z );
	RebuildTransform();
}

void D3DParticleEmitter::SetRotation( float x, float y, float z )
{
	m_Rotation = XMFLOAT3(x, y, z);
	RebuildTransform();
}

// Scale, Rotate, Translate the world matrix
void D3DParticleEmitter::RebuildTransform()
{
	// Set the current world matrix to the identity matrix
	XMMATRIXIdentity( &m_WorldMatrix );

	XMMATRIX temp;
	XMMATRIXIdentity( &temp );

	// Scale the temporary matrix
	XMMATRIXScaling( &temp, m_Scale.x, m_Scale.y, m_Scale.z );
	m_WorldMatrix *= temp;

	// Rotate the temporary matrix
	if (m_Rotation.x != 0) 
	{ XMMATRIXRotationX( &temp, m_Rotation.x ); m_WorldMatrix *= temp; }
	if (m_Rotation.y != 0) 
	{ XMMATRIXRotationY( &temp, m_Rotation.y ); m_WorldMatrix *= temp; }
	if (m_Rotation.z != 0) 
	{ XMMATRIXRotationZ( &temp, m_Rotation.z ); m_WorldMatrix *= temp; }

	// Translate the temporary matrix
	XMMATRIXTranslation( &temp, m_Position.x, m_Position.y, m_Position.z );
	m_WorldMatrix *= temp;
}

bool D3DParticleEmitter::Frame(float frameTime, ID3D11DeviceContext* deviceContext, D3DCamera* cam)
{
	bool result;

	// Release old particles.
	KillParticles();

	// Emit new particles.
	EmitParticles(frameTime);
	
	// Update the position of the particles.
	UpdateParticles(frameTime);

	// Update the dynamic vertex buffer with the new position of each particle.
	result = UpdateBuffers(deviceContext);
	if(!result)
	{
		return false;
	}

	return true;
}


void D3DParticleEmitter::Render(ID3D11DeviceContext* deviceContext)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType); 
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}


ID3D11ShaderResourceView* D3DParticleEmitter::GetTexture()
{
	return m_Texture->GetTexture();
}


int D3DParticleEmitter::GetIndexCount()
{
	return m_indexCount;
}


bool D3DParticleEmitter::LoadTexture(ID3D11Device* device, WCHAR* filename)
{
	bool result;


	// Create the texture object.
	m_Texture = new D3DTexture;
	if(!m_Texture)
	{
		return false;
	}

	// Initialize the texture object.
	result = m_Texture->Init(device, filename);
	if(!result)
	{
		return false;
	}

	return true;
}


void D3DParticleEmitter::ReleaseTexture()
{
	// Release the texture object.
	if(m_Texture)
	{
		delete m_Texture;
		m_Texture = 0;
	}

	return;
}


bool D3DParticleEmitter::InitializeParticleSystem()
{
	// Set the random deviation of where the particles can be located when emitted.
	m_particleDeviationX = 5.5f;
	m_particleDeviationY = 0.1f;
	m_particleDeviationZ = -5.0f;

	// Set the speed and speed variation of particles.
	m_particleVelocity = 1.5f;
	m_particleVelocityVariation = 0.3f;

	// Set the physical size of the particles.
	m_particleSize = 0.1f;

	// Set the number of particles to emit per second.
	m_particlesPerSecond = 900.0f;

	// Set the maximum number of particles allowed in the particle system.
	m_maxParticles = 5000;

	// Create the particle list.
	m_particleList = new ParticleType[m_maxParticles];
	if(!m_particleList)
	{
		return false;
	}

	// Initialize the particle list.
	for(int i = 0; i < m_maxParticles; i++)
	{
		m_particleList[i].active = false;
	}

	// Initialize the current particle count to zero since none are emitted yet.
	m_currentParticleCount = 0;

	// Clear the initial accumulated time for the particle per second emission rate.
	m_accumulatedTime = 0.0f;

	return true;
}


void D3DParticleEmitter::ShutdownParticleSystem()
{
	// Release the particle list.
	if(m_particleList)
	{
		delete [] m_particleList;
		m_particleList = 0;
	}

	return;
}


bool D3DParticleEmitter::InitializeBuffers(ID3D11Device* device)
{
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	// Set the maximum number of vertices's in the vertex array.
	m_vertexCount = m_maxParticles * 6;

	// Set the maximum number of indices in the index array.
	m_indexCount = m_vertexCount;

	// Create the vertex array for the particles that will be rendered.
	m_vertices = new VertexType[m_vertexCount];
	if(!m_vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[m_indexCount];
	if(!indices)
	{
		return false;
	}

	// Initialize vertex array to zeros at first.
	memset(m_vertices, 0, (sizeof(VertexType) * m_vertexCount));

	// Initialize the index array.
	for(int i =0 ; i < m_indexCount; i++)
	{
		indices[i] = i;
	}

	// Set up the description of the dynamic vertex buffer.
    vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
    vertexData.pSysMem = m_vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now finally create the vertex buffer.
    result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
    indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Release the index array since it is no longer needed.
	delete [] indices;
	indices = 0;

	return true;
}


void D3DParticleEmitter::ShutdownBuffers()
{
	// Release the index buffer.
	if(m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// Release the vertex buffer.
	if(m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}


void D3DParticleEmitter::EmitParticles(float frameTime)
{
	bool emitParticle, found;
	float positionX, positionY, positionZ, velocity;
	int index;


	// Increment the frame time.
	m_accumulatedTime += frameTime;

	// Set emit particle to false for now.
	emitParticle = false;
	
	// Check if it is time to emit a new particle or not.
	if(m_accumulatedTime > (1000.0f / m_particlesPerSecond))
	{
		m_accumulatedTime = 0.0f;
		emitParticle = true;
	}

	// If there are particles to emit then emit one per frame.
	if((emitParticle == true) && (m_currentParticleCount < (m_maxParticles - 1)))
	{
		m_currentParticleCount++;

		// Now generate the randomized particle properties.
		positionX = (((float)rand()-(float)rand())/RAND_MAX) * m_particleDeviationX;
		positionY = (((float)rand()-(float)rand())/RAND_MAX) * m_particleDeviationY;
		positionZ = (((float)rand()-(float)rand())/RAND_MAX) * m_particleDeviationZ;

		velocity = m_particleVelocity + (((float)rand()-(float)rand())/RAND_MAX) * m_particleVelocityVariation;
		// Now since the particles need to be rendered from back to front for blending we have to sort the particle array.
		// We will sort using Z depth so we need to find where in the list the particle should be inserted.
		index = 0;
		found = false;
		while(!found)
		{
			if((m_particleList[index].active == false) || (m_particleList[index].positionZ < positionZ))
				found = true;
			else
				index++;
		}

		// Now that we know the location to insert into we need to copy the array over by one position from the index to make room for the new particle.
		int i = m_currentParticleCount;
		int j = i - 1;

		while(i != index)
		{
			m_particleList[i].positionX = m_particleList[j].positionX;
			m_particleList[i].positionY = m_particleList[j].positionY;
			m_particleList[i].positionZ = m_particleList[j].positionZ;
			m_particleList[i].velocity  = m_particleList[j].velocity;
			m_particleList[i].active    = m_particleList[j].active;
			i--;
			j--;
		}

		// Now insert it into the particle array in the correct depth order.
		m_particleList[index].positionX = positionX;
		m_particleList[index].positionY = positionY;
		m_particleList[index].positionZ = positionZ;
		m_particleList[index].velocity  = velocity;
		m_particleList[index].active    = true;
	}

	return;
}


void D3DParticleEmitter::UpdateParticles(float frameTime)
{
	// Each frame we update all the particles by making them move downwards using their position, velocity, and the frame time.
	for(int i = 0; i <m_currentParticleCount; i++)
	{
		m_particleList[i].positionY = m_particleList[i].positionY - (m_particleList[i].velocity * frameTime * 0.001f);
	}

	return;
}


void D3DParticleEmitter::KillParticles()
{
	// Kill all the particles that have gone below a certain height range.
	for(int i = 0; i < m_maxParticles; i++)
	{
		if((m_particleList[i].active == true) && (m_particleList[i].positionY < -5.0f))
		{
			m_particleList[i].active = false;
			m_currentParticleCount--;

			// Now shift all the live particles back up the array to erase the destroyed particle and keep the array sorted correctly.
			for(int j = i; j<m_maxParticles-1; j++)
			{
				m_particleList[j].positionX = m_particleList[j+1].positionX;
				m_particleList[j].positionY = m_particleList[j+1].positionY;
				m_particleList[j].positionZ = m_particleList[j+1].positionZ;
				m_particleList[j].velocity  = m_particleList[j+1].velocity;
				m_particleList[j].active    = m_particleList[j+1].active;
			}
		}
	}
}


bool D3DParticleEmitter::UpdateBuffers(ID3D11DeviceContext* deviceContext)
{
	int index, i;
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* verticesPtr;


	// Initialize vertex array to zeros at first.
	memset(m_vertices, 0, (sizeof(VertexType) * m_vertexCount));

	// Now build the vertex array from the particle list array.  Each particle is a quad made out of two triangles.
	index = 0;

	for(i = 0; i < m_currentParticleCount; i++)
	{
		// Bottom left.
		m_vertices[index].position = XMFLOAT3(m_particleList[i].positionX - m_particleSize, m_particleList[i].positionY - m_particleSize, m_particleList[i].positionZ);
		m_vertices[index].texture = D3DXVECTOR2(0.0f, 1.0f);
		index++;

		// Top left.
		m_vertices[index].position = XMFLOAT3(m_particleList[i].positionX - m_particleSize, m_particleList[i].positionY + m_particleSize, m_particleList[i].positionZ);
		m_vertices[index].texture = D3DXVECTOR2(0.0f, 0.0f);
		index++;

		// Bottom right.
		m_vertices[index].position = XMFLOAT3(m_particleList[i].positionX + m_particleSize, m_particleList[i].positionY - m_particleSize, m_particleList[i].positionZ);
		m_vertices[index].texture = D3DXVECTOR2(1.0f, 1.0f);
		index++;

		// Bottom right.
		m_vertices[index].position = XMFLOAT3(m_particleList[i].positionX + m_particleSize, m_particleList[i].positionY - m_particleSize, m_particleList[i].positionZ);
		m_vertices[index].texture = D3DXVECTOR2(1.0f, 1.0f);
		index++;

		// Top left.
		m_vertices[index].position = XMFLOAT3(m_particleList[i].positionX - m_particleSize, m_particleList[i].positionY + m_particleSize, m_particleList[i].positionZ);
		m_vertices[index].texture = D3DXVECTOR2(0.0f, 0.0f);
		index++;

		// Top right.
		m_vertices[index].position = XMFLOAT3(m_particleList[i].positionX + m_particleSize, m_particleList[i].positionY + m_particleSize, m_particleList[i].positionZ);
		m_vertices[index].texture = D3DXVECTOR2(1.0f, 0.0f);
		index++;
	}
	
	// Lock the vertex buffer.
	result = deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the vertex buffer.
	verticesPtr = (VertexType*)mappedResource.pData;

	// Copy the data into the vertex buffer.
	memcpy(verticesPtr, (void*)m_vertices, (sizeof(VertexType) * m_vertexCount));

	// Unlock the vertex buffer.
	deviceContext->Unmap(m_vertexBuffer, 0);

	return true;
}