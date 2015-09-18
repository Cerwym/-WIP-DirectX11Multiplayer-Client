#include "D3DSkybox.h"

D3DSkyBox::D3DSkyBox()
{
	D3DXMatrixIdentity(&m_WorldMatrix);
	D3DXMatrixScaling(&m_ScaleMatrix, 5.0f, 5.0f, 5.0f);

	m_sampleState = 0;
}

D3DSkyBox::~D3DSkyBox()
{
	S_RELEASE(m_IndexBuffer);
	S_RELEASE(m_VertexBuffer);
	S_RELEASE(m_Texture);
	S_RELEASE(m_MatrixBuffer);

	S_RELEASE(SKYBOX_VS);
	S_RELEASE(SKYBOX_PS);
	S_RELEASE(SKYBOX_VS_Buffer);
	S_RELEASE(SKYBOX_PS_Buffer);

	S_RELEASE( m_sampleState );
}

bool D3DSkyBox::Init(ID3D11Device* d3d11Device, int LatLines, int LongLines, WCHAR* skyTexture, HWND hwnd)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;
	ID3D10Blob* errorMessage;

	// Initialize the pointers this function will use to null.
	errorMessage = 0;

	m_Vertices = ((LatLines-2) * LongLines) + 2;
	m_Faces = ((LatLines-3)*(LongLines)*2) + (LongLines*2);

	float sphereYaw = 0.0f;
	float spherePitch = 0.0f;

	std::vector<Vertex> vertices(m_Vertices);

	XMFLOAT3 currVertPos = XMFLOAT3(0.0f, 0.0f, 1.0f);

	vertices[0].Position.x = 0.0f;
	vertices[0].Position.y = 0.0f;
	vertices[0].Position.z = 1.0f;

	for(int i = 0; i < LatLines-2; ++i)
	{
		spherePitch = (float)((i+1) * (3.14/(LatLines-1)));
		D3DXMatrixRotationX(&m_RotationX, spherePitch);
		for(int j = 0; j < LongLines; ++j)
		{
			sphereYaw = (float)(j * (6.28/(LongLines)));
			D3DXMatrixRotationZ(&m_RotationY, sphereYaw);
			D3DXVec3TransformNormal( &currVertPos, &XMFLOAT3(0.0f, 0.0f, 1.0f), &(m_RotationX * m_RotationY) );
			D3DXVec3Normalize(&currVertPos, &currVertPos);
			
			vertices[i*LongLines+j+1].Position.x = currVertPos.x;
			vertices[i*LongLines+j+1].Position.y = currVertPos.y;
			vertices[i*LongLines+j+1].Position.z = currVertPos.z;
		}
	}

	vertices[m_Vertices-1].Position.x =  0.0f;
	vertices[m_Vertices-1].Position.y =  0.0f;
	vertices[m_Vertices-1].Position.z = -1.0f;


	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory( &vertexBufferDesc, sizeof(vertexBufferDesc) );

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof( Vertex ) * m_Vertices;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData; 

	ZeroMemory( &vertexBufferData, sizeof(vertexBufferData) );
	vertexBufferData.pSysMem = &vertices[0];
	d3d11Device->CreateBuffer( &vertexBufferDesc, &vertexBufferData, &m_VertexBuffer);

	std::vector<int> indices(m_Faces * 3);

	int k = 0;
	for(int l = 0; l < LongLines-1; ++l)
	{
		indices[k] = 0;
		indices[k+1] = l+1;
		indices[k+2] = l+2;
		k += 3;
	}

	indices[k] = 0;
	indices[k+1] = LongLines;
	indices[k+2] = 1;
	k += 3;

	for(int i = 0; i < LatLines-3; ++i)
	{
		for(int j = 0; j < LongLines-1; ++j)
		{
			indices[k]   = i*LongLines+j+1;
			indices[k+1] = i*LongLines+j+2;
			indices[k+2] = (i+1)*LongLines+j+1;

			indices[k+3] = (i+1)*LongLines+j+1;
			indices[k+4] = i*LongLines+j+2;
			indices[k+5] = (i+1)*LongLines+j+2;

			k += 6; // next quad
		}

		indices[k]   = (i*LongLines)+LongLines;
		indices[k+1] = (i*LongLines)+1;
		indices[k+2] = ((i+1)*LongLines)+LongLines;

		indices[k+3] = ((i+1)*LongLines)+LongLines;
		indices[k+4] = (i*LongLines)+1;
		indices[k+5] = ((i+1)*LongLines)+1;

		k += 6;
	}

	for(int l = 0; l < LongLines-1; ++l)
	{
		indices[k] = m_Vertices-1;
		indices[k+1] = (m_Vertices-1)-(l+1);
		indices[k+2] = (m_Vertices-1)-(l+2);
		k += 3;
	}

	indices[k] = m_Vertices-1;
	indices[k+1] = (m_Vertices-1)-LongLines;
	indices[k+2] = m_Vertices-2;

	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory( &indexBufferDesc, sizeof(indexBufferDesc) );

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(DWORD) * m_Faces * 3;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA iinitData;

	iinitData.pSysMem = &indices[0];
	d3d11Device->CreateBuffer(&indexBufferDesc, &iinitData, &m_IndexBuffer);

	if(FAILED(D3DX11CompileFromFile(L"Data/Shaders/skybox.fx", 0, 0, "SKYMAP_VS", "vs_5_0", 0, 0, 0, &SKYBOX_VS_Buffer, &errorMessage, 0)))
	{
		// If the shader failed to compile it should have written something to the error message.
		if(errorMessage)
			OutputShaderErrorMessage(errorMessage, hwnd, L"skybox.fx.fx");
		else
			MessageBox(hwnd, L"skybox.fx", L"Missing Shader File", MB_OK);
		
		return false;
	}
	
	if (FAILED(D3DX11CompileFromFile(L"Data/Shaders/skybox.fx", 0, 0, "SKYMAP_PS", "ps_5_0", 0, 0, 0, &SKYBOX_PS_Buffer, &errorMessage, 0)))
	{
		// If the shader failed to compile it should have written something to the error message.
		if(errorMessage)
			OutputShaderErrorMessage(errorMessage, hwnd, L"skybox.fx");
		else
			MessageBox(hwnd, L"skybox.fx", L"Missing Shader File", MB_OK);
		
		return false;
	}

	d3d11Device->CreateVertexShader(SKYBOX_VS_Buffer->GetBufferPointer(), SKYBOX_VS_Buffer->GetBufferSize(), NULL, &SKYBOX_VS);
	d3d11Device->CreatePixelShader(SKYBOX_PS_Buffer->GetBufferPointer(), SKYBOX_PS_Buffer->GetBufferSize(), NULL, &SKYBOX_PS);

	matrixBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	matrixBufferDesc.ByteWidth = sizeof(ConstantBuffer);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = 0;
	matrixBufferDesc.MiscFlags = 0;

	if (FAILED(d3d11Device->CreateBuffer(&matrixBufferDesc, NULL, &m_MatrixBuffer)))
	{
		MessageBox(hwnd, L"Failed Creating constant buffer", L"Error", MB_OK);
		return false;
	}
	
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	
	d3d11Device->CreateSamplerState( &samplerDesc, &m_sampleState );

	if (!LoadTexture(d3d11Device, skyTexture, hwnd))
		return false;

	return true;
}

bool D3DSkyBox::LoadTexture(ID3D11Device* d3d11Device, WCHAR* skyTexture, HWND hwnd)
{
	D3DX11_IMAGE_LOAD_INFO loadSMinfo;
	loadSMinfo.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	ID3D11Texture2D* SMTexture = 0;
	D3DX11CreateTextureFromFile(d3d11Device, skyTexture, &loadSMinfo, 0, (ID3D11Resource**)&SMTexture, 0);

	D3D11_TEXTURE2D_DESC SMTextureDesc;
	SMTexture->GetDesc(&SMTextureDesc);

	D3D11_SHADER_RESOURCE_VIEW_DESC SMViewDesc;
	SMViewDesc.Format = SMTextureDesc.Format;
	SMViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	SMViewDesc.TextureCube.MipLevels = SMTextureDesc.MipLevels;
	SMViewDesc.TextureCube.MostDetailedMip = 0;

	if (FAILED(d3d11Device->CreateShaderResourceView(SMTexture, &SMViewDesc, &m_Texture)))
	{
		MessageBox(hwnd, L"Failed to Load Texture", L"Skybox", MB_OK);
		return false;
	}
	return true;
}

void D3DSkyBox::Draw( ID3D11DeviceContext* deviceContext, D3DCamera* camera )
{
	// Make sure the skybox is always centred around the camera
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	unsigned int stride;
	unsigned int offset;
	stride = sizeof(Vertex);
	offset = 0;

	D3DXMatrixTranslation(&m_TranslationMatrix, camera->GetPosition().x, camera->GetPosition().y, camera->GetPosition().z);
	m_WorldMatrix = m_ScaleMatrix * m_TranslationMatrix;

	deviceContext->IASetIndexBuffer( m_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	//Set the spheres vertex buffer
	deviceContext->IASetVertexBuffers( 0, 1, &m_VertexBuffer, &stride, &offset );

	//Set the WVP matrix and send it to the constant buffer in effect file
	D3DXMATRIX WVP = m_WorldMatrix * camera->GetViewMatrix() * camera->GetProjMatrix();
	D3DXMatrixTranspose(&m_CB.WVP, &WVP);
	D3DXMatrixTranspose(&m_CB.World, &m_WorldMatrix);
	
	deviceContext->UpdateSubresource( m_MatrixBuffer, 0, NULL, &m_CB, 0, 0 );
	
	deviceContext->VSSetConstantBuffers( 0, 1, &m_MatrixBuffer );
	//Send our skymap resource view to pixel shader
	deviceContext->PSSetShaderResources( 0, 1, &m_Texture );
	deviceContext->PSSetSamplers( 0, 1, &m_sampleState );

	//Set the new VS and PS shaders
	deviceContext->VSSetShader(SKYBOX_VS, 0, 0);
	deviceContext->PSSetShader(SKYBOX_PS, 0, 0);
	//Set the new depth/stencil and RS states
	
	deviceContext->DrawIndexed( m_Faces * 3, 0, 0 );
}

void D3DSkyBox::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	CreateDirectory(L"Error Logs", NULL); // Create the directory for error messages, if it already exists, just ignore the return.
	char* compileErrors;
	char* filename = "Error Logs/SkyboxShader-error-";
	unsigned long bufferSize, i;
	int attempt;
	ofstream fout;
	attempt = 0;
	stringstream ss;
	string tmp;

	// Get a pointer to the error message text buffer.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// Get the length of the message.
	bufferSize = errorMessage->GetBufferSize();

	// Open a file to write the error message to.
	// Would like to increment filename

	// Check if file exists
	ss << filename << attempt << ".txt";
	tmp	= ss.str();
	while( _access( tmp.c_str(), 00 ) == 0 ) // 00 is existence only, 0 returns file exists
	{
		ss.str("");
		ss.clear();
		attempt++;
		ss << filename << attempt << ".txt"; 
		tmp = ss.str();
	}

	fout.open(ss.str());

	// Write out the error message.
	for(i=0; i<bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// Close the file.
	fout.close();

	// Release the error message.
	errorMessage->Release();
	errorMessage = 0;

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	MessageBox(hwnd, L"Error compiling shader.  Check the 'Error Logs' directory for error messages.", shaderFilename, MB_OK);
}