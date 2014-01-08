#include "D3DModel.h"
#include "..\Engine.h"

D3DModel::D3DModel()
{
	m_VertexBuffer = 0;
	m_indexBuffer = 0;
	m_Texture = 0;
	m_Model = 0;

	m_Scale = D3DXVECTOR3( 1.0f, 1.0f, 1.0f );
	m_Position = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_Rotation = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
}

D3DModel::D3DModel( D3DXVECTOR3& pos )
{
	m_VertexBuffer = 0;
	m_indexBuffer = 0;
	m_Texture = 0;
	m_Model = 0;

	m_Scale = D3DXVECTOR3( 1.0f, 1.0f, 1.0f );
	m_Position = D3DXVECTOR3( pos.x, pos.y, pos.z );
	m_Rotation = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
}

D3DModel::~D3DModel() 
{
	// Safely release the index and vertex buffers
	S_RELEASE(m_indexBuffer);
	S_RELEASE(m_VertexBuffer);
	// Safely release the texture and model objects
	S_DELETE(m_Texture);
	S_DELETE(m_Model);
}

bool D3DModel::Init(ID3D11Device* device, char* modelFName, WCHAR* textureFName)
{
	bool result;

	result = LoadModel(modelFName);
	if (!result)
		return false;
	// Initialize the vertex and index buffer that hold the geometry for the triangle
	result = InitBuffers(device);
	if (!result)
		return false;

	result = LoadTexture(device, textureFName);
	if(!result)
		return false;

	RebuildTransform();

	return true;
}

// Scale, Rotate, Translate the world matrix
void D3DModel::RebuildTransform()
{
	// Set the current world matrix to the identity matrix
	D3DXMatrixIdentity( &m_WorldMatrix );

	D3DXMATRIX temp;
	D3DXMatrixIdentity( &temp );
	
	// Scale the temporary matrix
	D3DXMatrixScaling( &temp, m_Scale.x, m_Scale.y, m_Scale.z );
	m_WorldMatrix *= temp;
	
	// Rotate the temporary matrix
	g_Engine->DebugOutput( L"Rotation Vector is %f, %f, %f\n", m_Rotation.x, m_Rotation.y, m_Rotation.z );
	if (m_Rotation.x != 0) 
	{ D3DXMatrixRotationX( &temp, m_Rotation.x ); m_WorldMatrix *= temp; }
	if (m_Rotation.y != 0) 
	{ D3DXMatrixRotationY( &temp, m_Rotation.y ); m_WorldMatrix *= temp; }
	if (m_Rotation.z != 0) 
	{ D3DXMatrixRotationZ( &temp, m_Rotation.z ); m_WorldMatrix *= temp; }
	
	// Translate the temporary matrix
	D3DXMatrixTranslation( &temp, m_Position.x, m_Position.y, m_Position.z );
	m_WorldMatrix *= temp;
}

void D3DModel::Render( ID3D11DeviceContext* deviceContext, D3DShaderManager* sMgr, D3DXMATRIX view, D3DXMATRIX proj )
{
	// Put the vertex and index buffers on the graphics pipeline so the colour shader will be able to render them
	unsigned int stride;
	unsigned int offset;

	stride = sizeof(VertexType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	sMgr->RenderTextureShader(deviceContext, m_indexCount, m_WorldMatrix, view, proj, m_Texture->GetTexture() );
}

void D3DModel::Render( ID3D11DeviceContext* deviceContext, D3DShaderManager* sMgr, D3DXMATRIX view, D3DXMATRIX proj, D3DXVECTOR3 cameraPos, D3DLight* lightObj )
{
	// Put the vertex and index buffers on the graphics pipeline so the colour shader will be able to render them
	unsigned int stride;
	unsigned int offset;

	stride = sizeof(VertexType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	sMgr->RenderLightShader(deviceContext, m_indexCount, m_WorldMatrix, view, proj, m_Texture->GetTexture(), cameraPos, lightObj );
}
/*

void D3DModel::Render( ID3D11DeviceContext* deviceContext, D3DShaderManager* sMgr, D3DXMATRIX view, D3DXMATRIX proj )
{
	// Put the vertex and index buffers on the graphics pipeline so the colour shader will be able to render them
	unsigned int stride;
	unsigned int offset;

	stride = sizeof(VertexType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	sMgr->RenderTextureShader(deviceContext, m_indexCount, m_WorldMatrix, view, proj, m_Texture->GetTexture() );
	sMgr->RenderLightShader(deviceContext, m_indexCount, m_WorldMatrix, view, proj, m_Texture->GetTexture(), )
}*/

int D3DModel::GetIndexCount()
{
	return m_indexCount;
}

void D3DModel::TranslateBy( float x, float y, float z )
{
	m_Position += D3DXVECTOR3( x, y, z );
	RebuildTransform();
}

void D3DModel::TranslateTo( float x, float y, float z )
{
	m_Position = D3DXVECTOR3( x, y, z );
	RebuildTransform();
}

void D3DModel::RotateBy( float x, float y, float z )
{
	m_Rotation += D3DXVECTOR3( x, y, z );
	RebuildTransform();
}

bool D3DModel::InitBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
		return false;

	// Create the index array.
	indices = new unsigned long[m_indexCount];
	if (!indices)
		return false;

	// Fill the index buffer with data
	for (int i = 0; i < m_vertexCount; i++)
	{
		vertices[i].position = D3DXVECTOR3(m_Model[i].x, m_Model[i].y, m_Model[i].z);
		vertices[i].texture = D3DXVECTOR2(m_Model[i].tu, m_Model[i].tv);
		vertices[i].normal = D3DXVECTOR3(m_Model[i].nx, m_Model[i].ny, m_Model[i].nz);
		indices[i] = i;
	}

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_VertexBuffer);
	if (FAILED(result))
		return false;

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
	if (FAILED(result))
		return false;

	// Release the arrays now that the vertex and index buffers have been created and loaded
	delete [] vertices;
	vertices = 0;

	delete [] indices;
	indices = 0;

	return true;
}

bool D3DModel::LoadTexture(ID3D11Device* device, WCHAR* fileName)
{
	bool result;

	m_Texture = new D3DTexture;
	if (!m_Texture)
		return false;

	result = m_Texture->Init(device, fileName);
	if (!result)
		return false;
	return true;
}

bool D3DModel::LoadModel(char* fName)
{
	ifstream fin;
	char input;

	// Open the model file.
	fin.open(fName);

	// If it could not open the file then exit.
	if(fin.fail())
		return false;

	// Read up to the value of vertex count.
	fin.get(input);
	while(input != ':')
		fin.get(input);

	// Read in the vertex count.
	fin >> m_vertexCount;

	// Set the number of indices to be the same as the vertex count.
	m_indexCount = m_vertexCount;

	// Create the model using the vertex count that was read in.
	m_Model = new ModelType[m_vertexCount];
	if(!m_Model)
		return false;

	// Read up to the beginning of the data.
	fin.get(input);
	while(input != ':')
		fin.get(input);

	fin.get(input);
	fin.get(input);

	// Read in the vertex data.
	for(int i = 0; i < m_vertexCount; i++)
	{
		fin >> m_Model[i].x >> m_Model[i].y >> m_Model[i].z;
		fin >> m_Model[i].tu >> m_Model[i].tv;
		fin >> m_Model[i].nx >> m_Model[i].ny >> m_Model[i].nz;
	}

	// Close the model file.
	fin.close();

	return true;
}