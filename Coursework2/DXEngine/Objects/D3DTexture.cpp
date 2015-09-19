#include "D3DTexture.h"

D3DTexture::D3DTexture()
{
	m_Texture = 0;
	m_TextureView = 0;
	m_TGAImage = 0;
}

D3DTexture::D3DTexture(ID3D11ShaderResourceView* resource )
{
	m_TextureView = resource;
}

D3DTexture::~D3DTexture()
{
	S_RELEASE(m_Texture);
	S_RELEASE(m_TextureView);
	

	if (m_TGAImage)
	{
		delete[] m_TGAImage;
		m_TGAImage = 0;
	}
}

bool D3DTexture::Init(ID3D11Device* device, ID3D11DeviceContext* context, char* filename)
{
	bool result;
	int height, width;
	D3D11_TEXTURE2D_DESC textureDesc;
	HRESULT hResult;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	unsigned char* dataPtr;
	unsigned int i, j, k, rowStart, columnStart;
	D3D11_SHADER_RESOURCE_VIEW_DESC srDesc;


	// Load the targa texture data into memory.
	result = LoadTGA(filename, height, width);
	if (!result)
	{
		return false;
	}

	// Setup the description of the texture.
	textureDesc.Height = height;
	textureDesc.Width = width;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DYNAMIC;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	textureDesc.MiscFlags = 0;

	// Create the empty texture.
	hResult = device->CreateTexture2D(&textureDesc, NULL, &m_Texture);
	if (FAILED(hResult))
	{
		return false;
	}

	// Lock the texture so it can be written to by the CPU.
	hResult = context->Map(m_Texture, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(hResult))
	{
		return false;
	}

	// Get a pointer to the mapped resource data pointer and cast it into an unsigned char to match the targa data.
	dataPtr = (unsigned char*)mappedResource.pData;

	// Initialize the index into the targa data since targa data is stored upside down.
	k = (width * height * 4) - (width * 4);

	// Load the targa data into the texture now.
	for (j = 0; j<(unsigned int)height; j++)
	{
		// Set the beginning of the row.
		rowStart = j * mappedResource.RowPitch;

		for (i = 0; i<(unsigned int)width; i++)
		{
			// Set the beginning of the column.
			columnStart = i * 4;

			// Copy the data in.
			dataPtr[rowStart + columnStart + 0] = m_TGAImage[k + 2];  // Red.
			dataPtr[rowStart + columnStart + 1] = m_TGAImage[k + 1];  // Green.
			dataPtr[rowStart + columnStart + 2] = m_TGAImage[k + 0];  // Blue
			dataPtr[rowStart + columnStart + 3] = m_TGAImage[k + 3];  // Alpha

			// Increment the index into the targa data.
			k += 4;
		}

		// Set the targa data index back to the preceding row at the beginning of the column since its reading it in upside down.
		k -= (width * 8);
	}

	// Unlock the texture.
	context->Unmap(m_Texture, 0);

	// Release the targa image data now that it has been loaded into the texture.
	delete[] m_TGAImage;
	m_TGAImage = 0;

	// Create the shader-resource view
	srDesc.Format = textureDesc.Format;
	srDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srDesc.Texture2D.MostDetailedMip = 0;
	srDesc.Texture2D.MipLevels = 1;

	// Create the shader resource view for the texture.
	hResult = device->CreateShaderResourceView(m_Texture, &srDesc, &m_TextureView);
	if (FAILED(hResult))
	{
		return false;
	}

	return true;
}

bool D3DTexture::LoadTGA(char* filename, int &height, int &width)
{
	int error, bpp, imageSize;
	FILE* filePtr;
	unsigned int count;
	TGAHeader targaFileHeader;


	// Open the targa file for reading in binary.
	error = fopen_s(&filePtr, filename, "rb");
	if (error != 0)
	{
		return false;
	}

	// Read in the file header.
	count = fread(&targaFileHeader, sizeof(TGAHeader), 1, filePtr);
	if (count != 1)
	{
		return false;
	}

	// Get the important information from the header.
	height = (int)targaFileHeader.height;
	width = (int)targaFileHeader.width;
	bpp = (int)targaFileHeader.bpp;

	// Check that it is 32 bit and not 24 bit.
	if (bpp != 32)
	{
		return false;
	}

	// Calculate the size of the 32 bit image data.
	imageSize = width * height * 4;

	// Allocate memory for the targa image data.
	m_TGAImage = new unsigned char[imageSize];
	if (!m_TGAImage)
	{
		return false;
	}

	// Read in the targa image data.
	count = fread(m_TGAImage, 1, imageSize, filePtr);
	if (count != imageSize)
	{
		return false;
	}

	// Close the file.
	error = fclose(filePtr);
	if (error != 0)
	{
		return false;
	}

	return true;
}