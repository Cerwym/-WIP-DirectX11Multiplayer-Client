#include "D3DBitmap.h"

D3DBitmap::D3DBitmap()
{
	m_vertexBuffer = 0;
	m_IndexBuffer = 0;
	m_Texture = 0;
}

D3DBitmap::~D3DBitmap()
{
	// Release the index and vertex buffers
	S_RELEASE(m_IndexBuffer);
	S_RELEASE(m_vertexBuffer);
	// Delete the texture
	S_DELETE( m_Texture );
}

bool D3DBitmap::Init(ID3D11Device* device, ID3D11DeviceContext* context, int screenWidth, int screenHeight, char* bitmapFName, int bitmapWidth, int bitmapHeight)
{
	cbEvery.m_worldMatrix = XMMatrixIdentity();

	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	m_bitmapWidth = bitmapWidth;
	m_bitmapHeight = bitmapHeight;

	// Store the location of where the image was last drawn, if the location hasn't changed between frames, do not modify the dynamic vertex buffer for this instance
	m_previousPosX = -1;
	m_previousPosY = -1;

	if (!InitBuffers(device))
		return false;

	if (bitmapFName!= NULL) // if the filename of the bitmap is not NULL, that means we're passing in external data, i.e a file
	{
		if (LoadTexture(device, context, bitmapFName))
			return false;

	}
	
	return true;
}

bool D3DBitmap::Render(ID3D11DeviceContext* deviceContext, D3DShaderManager* sm, const XMMATRIX &view, const XMMATRIX &proj)
{
	bool result;

	// A texture cannot be drawn if the object is null
	assert(m_Texture != NULL && "Cannot draw a NULL texture");

	result = UpdateBuffers(deviceContext, m_Position.x, m_Position.y);
	if (!result)
		return false;

	RenderBuffers(deviceContext);

	sm->RenderTextureShader(deviceContext, m_indexCount, cbEvery.m_worldMatrix, view, proj, m_Texture->GetTexture() );
	
	return true;
}

bool D3DBitmap::InitBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	m_vertexCount = 6;
	m_indexCount = m_vertexCount;
	vertices = new VertexType[m_vertexCount];
	if(!vertices)
		return false;

	indices = new unsigned long[m_indexCount];
	if (!indices)
		return false;

	// Init vertex array to zeros
	memset(vertices, 0, (sizeof(VertexType) * m_vertexCount));

	for (int i = 0; i < m_indexCount; i++)
		indices[i] = i;

	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if(FAILED(result))
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
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_IndexBuffer);
	if(FAILED(result))
		return false;
	
	delete [] vertices;
	vertices = 0;

	delete [] indices;
	indices = 0;

	return true;
}

bool D3DBitmap::UpdateBuffers(ID3D11DeviceContext* deviceContext, int posX, int posY)
{
	float left, right, top, bottom;
	VertexType* verts;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* vertPtr;
	HRESULT result;

	// Check to see if the position has changed, if it hasn't return and forgo further processing.
	if ( (posX == m_previousPosX) && (posY == m_previousPosY))
		return true;

	// If it has changed, update its position and recalculate the sides on the image and is appropriate vertex, index data
	m_previousPosX = posX;
	m_previousPosY = posY;

	left = (float)((m_screenWidth / 2) * -1) + (float)posX;
	right = left + (float)m_bitmapWidth;
	top = (float)(m_screenHeight / 2) - (float)posY;
	bottom = top - (float)m_bitmapHeight;

	verts = new VertexType[m_vertexCount];
	if (!verts)
		return false;

	// First triangle data
	verts[0].position = XMFLOAT3(left, top, 0.0f);  // Top left.
	verts[0].texture = XMFLOAT2(0.0f, 0.0f);

	verts[1].position = XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
	verts[1].texture = XMFLOAT2(1.0f, 1.0f);

	verts[2].position = XMFLOAT3(left, bottom, 0.0f);  // Bottom left.
	verts[2].texture = XMFLOAT2(0.0f, 1.0f);

	// Second triangle.
	verts[3].position = XMFLOAT3(left, top, 0.0f);  // Top left.
	verts[3].texture = XMFLOAT2(0.0f, 0.0f);

	verts[4].position = XMFLOAT3(right, top, 0.0f);  // Top right.
	verts[4].texture = XMFLOAT2(1.0f, 0.0f);

	verts[5].position = XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
	verts[5].texture = XMFLOAT2(1.0f, 1.0f);

	// Lock the vertex buffer for writing
	result = deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
		return false;

	vertPtr = (VertexType*)mappedResource.pData;
	memcpy(vertPtr, (void*)verts, (sizeof(VertexType) * m_vertexCount));

	// Unlock the vertex buffer
	deviceContext->Unmap(m_vertexBuffer, 0);

	delete [] verts;
	verts = 0;

	return true;
}

void D3DBitmap::RenderBuffers( ID3D11DeviceContext* deviceContext )
{
	unsigned int stride;
	unsigned int offset = 0;

	stride = sizeof(VertexType);

	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	//deviceContext->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R8G8B8A8_UINT, 0);
	deviceContext->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

bool D3DBitmap::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* context, char* fName)
{
	bool result;
	m_Texture = new D3DTexture;
	if (!m_Texture)
		return false;

	result = m_Texture->Init(device, context, fName);
	if (!result)
		return false;

	return true;
}

void D3DBitmap::SetTexture( ID3D11ShaderResourceView* texture )
{
	// Delete the texture if it already exists as we don't want dangling pointers
	if (m_Texture)
	{
		delete m_Texture;
		m_Texture = 0;
	}

	 m_Texture = new D3DTexture(texture);
}