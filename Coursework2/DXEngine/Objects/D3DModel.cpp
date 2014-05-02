#include "D3DModel.h"
#include "..\Engine.h"

D3DModel::D3DModel()
{
	m_VertexBuffer = 0;
	m_indexBuffer = 0;
	m_Texture = 0;
	m_Model = 0;
	m_BModel = 0;
	m_NormalMapTexture = 0;
	m_instanceCount = 0;
	m_instanceBuffer = 0;

	m_isBumpMapped = false;

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
	m_BModel = 0;
	m_NormalMapTexture = 0;
	m_instanceCount = 0;
	m_instanceBuffer = 0;

	m_isBumpMapped = false;

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
	S_DELETE(m_BModel);
	S_DELETE(m_NormalMapTexture);
}

bool D3DModel::Init( ID3D11Device* device, char* modelFName, WCHAR* textureFName, WCHAR* bumpMapFname )
{
	bool result;
	if (bumpMapFname != NULL) { m_isBumpMapped = true; }

	result = LoadModel(modelFName, m_isBumpMapped);
	if (!result)
	{
		MessageBox(g_Engine->GetWindow(), L"Error loading model", L"Error", MB_OK);
		return false;
	}

	// Calculate the tangent and binormal vectors for the model if the bump map texture has been passed, if not skip to save process time
	if ( m_isBumpMapped )
	{
		CalculateModelVectors();
	}
	
	// Initialize the vertex and index buffer that hold the geometry for the triangle
	result = InitBuffers(device);
	if (!result)
		return false;

	result = LoadTexture(device, textureFName, bumpMapFname);
	if(!result)
		return false;

	// Check to see if the bump map has been passed

	RebuildTransform();

	return true;
}

void D3DModel::NO_GSBillboard(D3DCamera* camera)
{
	double angle = atan2( m_Position.x - camera->GetPosition().x, m_Position.z - camera->GetPosition().z) * ( 180 / D3DX_PI );
	float rotation = angle * 0.0174532925f;
	SetRotation( 0, rotation, 0 );
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

bool D3DModel::Render( ID3D11DeviceContext* deviceContext, D3DShaderManager* sMgr, D3DCamera* camera, D3DLight* lightObj, const int cullMode )
{

	// Before doing any pipeline operations, check to see if the object is inside the camera's Frustum
	// TODO : extend this to have the ability to set CULL_MODE on this object so it's not always checking for cube-objects
	if ( cullMode != CULL_NEVER )
	{
		if ( !camera->GetFrustum()->CheckCube( m_Position.x, m_Position.y, m_Position.z, 1.0f) )
		{
			return false; // There is no need to put the index and buffers on the pipeline because the object is not going to be rendered
		}
	}

	// Put the vertex and index buffers on the graphics pipeline so the colour shader will be able to render them
	unsigned int stride;
	unsigned int offset;

	if (!m_isBumpMapped)
		stride = sizeof(TexVertex);
	else
		stride = sizeof(BumpVertex);

	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	if (!m_isBumpMapped)
	{
		if (lightObj == NULL)
			sMgr->RenderTextureShader(deviceContext, m_indexCount, m_WorldMatrix, camera->GetViewMatrix(), camera->GetProjMatrix(), m_Texture->GetTexture() );
		else
			sMgr->RenderLightShader(deviceContext, m_indexCount, m_WorldMatrix, camera->GetViewMatrix(), camera->GetProjMatrix(), m_Texture->GetTexture(), camera->GetPosition(), lightObj );
	}
	else
		sMgr->RenderBumpMapShader(deviceContext, m_indexCount, m_WorldMatrix, camera->GetViewMatrix(), camera->GetProjMatrix(), m_Texture->GetTexture(), m_NormalMapTexture->GetTexture(), lightObj );

	return true; // Object was on screen and rendered
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

void D3DModel::SetRotation( float x, float y, float z )
{
	m_Rotation = D3DXVECTOR3(x, y, z);
	RebuildTransform();
}

bool D3DModel::InitBuffers(ID3D11Device* device)
{
	TexVertex* vertices = 0;
	BumpVertex* Bvertices = 0;
	InstancedData* instances = 0;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc, instanceBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData, instanceData;
	HRESULT result;

	// Create the vertex array.

	if (m_isBumpMapped == false)
	{
		vertices = new TexVertex[m_vertexCount];
		if (!vertices)
			return false;
	}
	else
	{
		Bvertices = new BumpVertex[m_vertexCount];
		if (!Bvertices)
			return false;
	}

	// Create the index array.
	indices = new unsigned long[m_indexCount];
	if (!indices)
		return false;

	if (m_isBumpMapped == false)
	{
		// Fill the index buffer with data
		for (int i = 0; i < m_vertexCount; i++)
		{
			vertices[i].position = D3DXVECTOR3(m_Model[i].x, m_Model[i].y, m_Model[i].z);
			vertices[i].texture = D3DXVECTOR2(m_Model[i].tu, m_Model[i].tv);
			vertices[i].normal = D3DXVECTOR3(m_Model[i].nx, m_Model[i].ny, m_Model[i].nz);
			indices[i] = i;
		}
	}
	else
	{
		for( int i = 0; i < m_vertexCount; i++ )
		{
			Bvertices[i].position = D3DXVECTOR3(m_BModel[i].x, m_BModel[i].y, m_BModel[i].z);
			Bvertices[i].texture = D3DXVECTOR2(m_BModel[i].tu, m_BModel[i].tv);
			Bvertices[i].normal = D3DXVECTOR3(m_BModel[i].nx, m_BModel[i].ny, m_BModel[i].nz);
			Bvertices[i].tangent = D3DXVECTOR3(m_BModel[i].tx, m_BModel[i].ty, m_BModel[i].tz);
			Bvertices[i].binormal = D3DXVECTOR3(m_BModel[i].bx, m_BModel[i].by, m_BModel[i].bz);

			indices[i] = i;
		}
	}

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	if (!m_BModel)
		vertexBufferDesc.ByteWidth = sizeof(TexVertex) * m_vertexCount;
	else
		vertexBufferDesc.ByteWidth = sizeof(BumpVertex) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data
	if (!m_isBumpMapped)
		vertexData.pSysMem = vertices;
	else
		vertexData.pSysMem = Bvertices;

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
	if (vertices)
	{
		delete [] vertices;
		vertices = 0;
	}

	if (Bvertices)
	{
		delete [] indices;
		indices = 0;
	}

	delete [] indices;
	indices = 0;

	return true;
}

bool D3DModel::LoadTexture(ID3D11Device* device, WCHAR* fileName, WCHAR* bumpFN)
{
	bool result;

	m_Texture = new D3DTexture;
	if (!m_Texture)
		return false;

	result = m_Texture->Init(device, fileName);
	if (!result)
		return false;

	if (bumpFN != NULL)
	{
		m_NormalMapTexture = new D3DTexture;
		if (!m_NormalMapTexture)
			return false;

		if (!m_NormalMapTexture->Init(device, bumpFN))
			return false;
	}
	return true;
}

bool D3DModel::LoadModel(char* fName, bool bumpFlag)
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
	if (m_isBumpMapped == false)
	{
		m_Model = new TexModel[m_vertexCount];
		if(!m_Model)
			return false;
	}
	else
	{
		m_BModel = new BumpModel[m_vertexCount];
		if (!m_BModel)
			return false;
	}

	// Read up to the beginning of the data.
	fin.get(input);
	while(input != ':')
		fin.get(input);

	fin.get(input);
	fin.get(input);

	if (!m_isBumpMapped)
	{
		for(int i = 0; i < m_vertexCount; i++)
		{
			fin >> m_Model[i].x >> m_Model[i].y >> m_Model[i].z;
			fin >> m_Model[i].tu >> m_Model[i].tv;
			fin >> m_Model[i].nx >> m_Model[i].ny >> m_Model[i].nz;
		}
	}

	else
	{
		for(int i = 0; i < m_vertexCount; i++)
		{
			fin >> m_BModel[i].x >> m_BModel[i].y >> m_BModel[i].z;
			fin >> m_BModel[i].tu >> m_BModel[i].tv;
			fin >> m_BModel[i].nx >> m_BModel[i].ny >> m_BModel[i].nz;
		}
	}

	// Close the model file.
	fin.close();

	return true;
}

void D3DModel::CalculateModelVectors()
{
	int faceCount, index;
	TexModel vertex1, vertex2, vertex3;
	Vector3F tangent, binormal;


	// Calculate the number of faces in the model.
	faceCount = m_vertexCount / 3;

	// Initialize the index to the model data.
	index = 0;

	// Go through all the faces and calculate the the tangent, binormal, and normal vectors.
	for(int i = 0; i < faceCount; i++)
	{
		// Get the three vertices for this face from the model.
		vertex1.x = m_BModel[index].x;
		vertex1.y = m_BModel[index].y;
		vertex1.z = m_BModel[index].z;
		vertex1.tu = m_BModel[index].tu;
		vertex1.tv = m_BModel[index].tv;
		vertex1.nx = m_BModel[index].nx;
		vertex1.ny = m_BModel[index].ny;
		vertex1.nz = m_BModel[index].nz;
		index++;
		vertex2.x = m_BModel[index].x;
		vertex2.y = m_BModel[index].y;
		vertex2.z = m_BModel[index].z;
		vertex2.tu = m_BModel[index].tu;
		vertex2.tv = m_BModel[index].tv;
		vertex2.nx = m_BModel[index].nx;
		vertex2.ny = m_BModel[index].ny;
		vertex2.nz = m_BModel[index].nz;
		index++;

		vertex3.x = m_BModel[index].x;
		vertex3.y = m_BModel[index].y;
		vertex3.z = m_BModel[index].z;
		vertex3.tu = m_BModel[index].tu;
		vertex3.tv = m_BModel[index].tv;
		vertex3.nx = m_BModel[index].nx;
		vertex3.ny = m_BModel[index].ny;
		vertex3.nz = m_BModel[index].nz;
		index++;

		// Calculate the tangent and binormal of that face.
		CalculateTangentBinormal(vertex1, vertex2, vertex3, tangent, binormal);

		// Store the normal, tangent, and binormal for this face back in the model structure.
		m_BModel[index-1].tx = tangent.x;
		m_BModel[index-1].ty = tangent.y;
		m_BModel[index-1].tz = tangent.z;
		m_BModel[index-1].bx = binormal.x;
		m_BModel[index-1].by = binormal.y;
		m_BModel[index-1].bz = binormal.z;

		m_BModel[index-2].tx = tangent.x;
		m_BModel[index-2].ty = tangent.y;
		m_BModel[index-2].tz = tangent.z;
		m_BModel[index-2].bx = binormal.x;
		m_BModel[index-2].by = binormal.y;
		m_BModel[index-2].bz = binormal.z;

		m_BModel[index-3].tx = tangent.x;
		m_BModel[index-3].ty = tangent.y;
		m_BModel[index-3].tz = tangent.z;
		m_BModel[index-3].bx = binormal.x;
		m_BModel[index-3].by = binormal.y;
		m_BModel[index-3].bz = binormal.z;
	}

	return;
}


void D3DModel::CalculateTangentBinormal(TexModel vertex1, TexModel vertex2, TexModel vertex3,
											  Vector3F& tangent, Vector3F& binormal)
{
	float vector1[3], vector2[3];
	float tuVector[2], tvVector[2];
	float den;
	float length;

	// Calculate the two vectors for this face.
	vector1[0] = vertex2.x - vertex1.x;
	vector1[1] = vertex2.y - vertex1.y;
	vector1[2] = vertex2.z - vertex1.z;

	vector2[0] = vertex3.x - vertex1.x;
	vector2[1] = vertex3.y - vertex1.y;
	vector2[2] = vertex3.z - vertex1.z;

	// Calculate the tu and tv texture space vectors.
	tuVector[0] = vertex2.tu - vertex1.tu;
	tvVector[0] = vertex2.tv - vertex1.tv;

	tuVector[1] = vertex3.tu - vertex1.tu;
	tvVector[1] = vertex3.tv - vertex1.tv;

	// Calculate the denominator of the tangent/binormal equation.
	den = 1.0f / (tuVector[0] * tvVector[1] - tuVector[1] * tvVector[0]);

	// Calculate the cross products and multiply by the coefficient to get the tangent and binormal.
	tangent.x = (tvVector[1] * vector1[0] - tvVector[0] * vector2[0]) * den;
	tangent.y = (tvVector[1] * vector1[1] - tvVector[0] * vector2[1]) * den;
	tangent.z = (tvVector[1] * vector1[2] - tvVector[0] * vector2[2]) * den;

	binormal.x = (tuVector[0] * vector2[0] - tuVector[1] * vector1[0]) * den;
	binormal.y = (tuVector[0] * vector2[1] - tuVector[1] * vector1[1]) * den;
	binormal.z = (tuVector[0] * vector2[2] - tuVector[1] * vector1[2]) * den;

	// Calculate the length of this normal.
	length = sqrt((tangent.x * tangent.x) + (tangent.y * tangent.y) + (tangent.z * tangent.z));

	// Normalize the normal and then store it
	tangent.x = tangent.x / length;
	tangent.y = tangent.y / length;
	tangent.z = tangent.z / length;

	// Calculate the length of this normal.
	length = sqrt((binormal.x * binormal.x) + (binormal.y * binormal.y) + (binormal.z * binormal.z));

	// Normalize the normal and then store it
	binormal.x = binormal.x / length;
	binormal.y = binormal.y / length;
	binormal.z = binormal.z / length;

	return;
}