#include "D3DFontShader.h"

D3DFontShader::D3DFontShader()
{
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_layout = 0;
	m_constantBuffer = 0;
	m_sampleState = 0;
	m_pixelBuffer = 0;
}

D3DFontShader::~D3DFontShader()
{
	// Safely release all COMs
	S_RELEASE(m_pixelBuffer);
	S_RELEASE(m_sampleState);
	S_RELEASE(m_constantBuffer);
	S_RELEASE(m_layout);
	S_RELEASE(m_pixelShader);
	S_RELEASE(m_vertexShader);
}

bool D3DFontShader::Init(ID3D11Device* device, HWND hwnd)
{
	if (!InitShader( device, hwnd,  L"Data/Shaders/font.vs", L"Data/Shaders/font.ps" ) )
		return false;
	return true;
}

bool D3DFontShader::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, 
							 XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, XMFLOAT4 pixelColour)
{
	if (!SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture,pixelColour))
		return false;

	RenderShader(deviceContext, indexCount);

	return true;
}

bool D3DFontShader::InitShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFName, WCHAR* psFName)
{
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC constantBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC pixelBufferDesc;

	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	// Compile the vertex shader
	if (FAILED(D3DX11CompileFromFile(vsFName, NULL, NULL, "FontVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &vertexShaderBuffer, &errorMessage, NULL)))
	{
		if (errorMessage)
			OutputShaderErrorMessage(errorMessage, hwnd, vsFName);
		else
			MessageBox(hwnd, vsFName, L"Missing Shader File", MB_OK);

		return false;
	}

	// Compile the pixel shader.
	if(FAILED(D3DX11CompileFromFile(psFName, NULL, NULL, "FontPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &pixelShaderBuffer, &errorMessage, NULL)))
	{
		if(errorMessage)
			OutputShaderErrorMessage(errorMessage, hwnd, psFName);
		else
			MessageBox(hwnd, psFName, L"Missing Shader File", MB_OK);

		return false;
	}

	// Create the vertex shader from the buffer.
	if(FAILED(device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader)))
		return false;

	// Create the vertex shader from the buffer.
	if(FAILED(device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader)))
		return false;

	// Create the vertex input layout description.
	// This setup needs to match the VertexType structure in the ModelClass and in the shader.
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	// Get a count of the elements in the layout.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the vertex input layout.
	if(FAILED(device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout)))
		return false;

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// Setup the description of the dynamic constant buffer that is in the vertex shader.
	constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	constantBufferDesc.ByteWidth = sizeof(ConstantBufferType);
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constantBufferDesc.MiscFlags = 0;
	constantBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	if(FAILED(device->CreateBuffer(&constantBufferDesc, NULL, &m_constantBuffer)))
		return false;

	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	if(FAILED(device->CreateSamplerState(&samplerDesc, &m_sampleState)))
		return false;

	// Setup the description of the dynamic pixel constant buffer that is in the pixel shader.
	pixelBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	pixelBufferDesc.ByteWidth = sizeof(PixelBufferType);
	pixelBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	pixelBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pixelBufferDesc.MiscFlags = 0;
	pixelBufferDesc.StructureByteStride = 0;

	// Create the pixel constant buffer pointer so we can access the pixel shader constant buffer from within this class.
	if(FAILED(device->CreateBuffer(&pixelBufferDesc, NULL, &m_pixelBuffer)))
		return false;

	return true;
}

void D3DFontShader::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long bufferSize;
	ofstream f;

	// Get a pointer to the error message text buffer.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// Get the length of the message.
	bufferSize = errorMessage->GetBufferSize();

	// Open a file to write the error message to.
	f.open("shader-error.txt");

	// Write out the error message.
	for(unsigned long i = 0; i < bufferSize; i++)
	{
		f << compileErrors[i];
	}

	// Close the file.
	f.close();

	errorMessage->Release();
	errorMessage = 0;

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);
}

bool D3DFontShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, 
										  XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, XMFLOAT4 pixelColour)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ConstantBufferType* dataPtr;
	unsigned int bufferNumber;
	PixelBufferType* dataPtr2;


	// Lock the constant buffer so it can be written to.
	if(FAILED(deviceContext->Map(m_constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
		return false;

	// Get a pointer to the data in the constant buffer.
	dataPtr = (ConstantBufferType*)mappedResource.pData;

	// Transpose the matrices to prepare them for the shader.
	XMMATRIXTranspose(&worldMatrix, &worldMatrix);
	XMMATRIXTranspose(&viewMatrix, &viewMatrix);
	XMMATRIXTranspose(&projectionMatrix, &projectionMatrix);

	// Copy the matrices into the constant buffer.
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	// Unlock the constant buffer.
	deviceContext->Unmap(m_constantBuffer, 0);

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	// Now set the constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_constantBuffer);

	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &texture);
	// Lock the pixel constant buffer so it can be written to.
	if(FAILED(deviceContext->Map(m_pixelBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
		return false;

	// Get a pointer to the data in the pixel constant buffer.
	dataPtr2 = (PixelBufferType*)mappedResource.pData;

	// Copy the pixel color into the pixel constant buffer.
	dataPtr2->pixelColour = pixelColour;

	// Unlock the pixel constant buffer.
	deviceContext->Unmap(m_pixelBuffer, 0);

	// Set the position of the pixel constant buffer in the pixel shader.
	bufferNumber = 0;

	// Now set the pixel constant buffer in the pixel shader with the updated value.
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_pixelBuffer);

	return true;
}

void D3DFontShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// Set the vertex input layout.
	deviceContext->IASetInputLayout(m_layout);

	// Set the vertex and pixel shaders that will be used to render the triangles.
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// Set the sampler state in the pixel shader.
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);

	// Render the triangles.
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}