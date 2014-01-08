#include "D3DAudio.h"

D3DAudio::D3DAudio()
{
	m_DirectSound = 0;
	m_primaryBuffer = 0;
	m_secondaryBuffer1 = 0;
}

D3DAudio::~D3DAudio()
{
	//Todo Change this to shut down all buffers
	ShutdownWaveFile(&m_secondaryBuffer1);
	S_RELEASE(m_primaryBuffer);
	S_RELEASE(m_DirectSound);
}

bool D3DAudio::Init(HWND hwnd)
{
	m_hwnd = hwnd;
	// Init DirectSound and the primary buffer
	if (!InitDirectSound(hwnd))
		return false;

	return true;
}

void D3DAudio::Cleanup()
{
	// Might want to extend the buffer to have flags to determine if the instance is playing or should be destroyed
	ShutdownWaveFile(&m_secondaryBuffer1);
}

bool D3DAudio::InitDirectSound(HWND hwnd)
{
	DSBUFFERDESC bufferDesc;
	WAVEFORMATEX waveFormat;

	// Init the interface for the default sound device
	if (FAILED(DirectSoundCreate8(NULL, &m_DirectSound, NULL)))
		return false;

	// Set the cooperative level to priority so the format of the primary sound buffer can be modified
	if (FAILED(m_DirectSound->SetCooperativeLevel(hwnd, DSSCL_PRIORITY)))
		return false;

	// Setup the primary buffer description.
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME;
	bufferDesc.dwBufferBytes = 0;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = NULL;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	// Get control of the primary sound buffer on the default sound device.
	if(FAILED(m_DirectSound->CreateSoundBuffer(&bufferDesc, &m_primaryBuffer, NULL)))
		return false;

	// Setup the format of the primary sound buffer.
	// In this case it is a .WAV file recorded at 44,100 samples per second in 16-bit stereo (cd audio format).
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = 44100;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nChannels = 2;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	// Set the primary buffer to be the wave format specified.
	if(FAILED(m_primaryBuffer->SetFormat(&waveFormat)))
		return false;

	return true;
}

bool D3DAudio::LoadWaveFile(char* filename, IDirectSoundBuffer8** secondaryBuffer)
{
	FILE* filePtr;
	WaveHeaderType waveFileHeader;
	WAVEFORMATEX waveFormat;
	DSBUFFERDESC bufferDesc;
	IDirectSoundBuffer* tempBuffer;
	unsigned char* waveData;
	unsigned char *bufferPtr;
	unsigned long bufferSize;

	// Open the wave file in binary.
	if(fopen_s(&filePtr, filename, "rb") != 0)
		return false;

	// Read in the wave file header.
	if(fread(&waveFileHeader, sizeof(waveFileHeader), 1, filePtr) != 1)
	{
		return false;
	}

	// Check that the chunk ID is the RIFF format.
	if((waveFileHeader.chunkID[0] != 'R') || (waveFileHeader.chunkID[1] != 'I') || 
		(waveFileHeader.chunkID[2] != 'F') || (waveFileHeader.chunkID[3] != 'F'))
	{
		return false;
	}

	// Check that the file format is the WAVE format.
	if((waveFileHeader.format[0] != 'W') || (waveFileHeader.format[1] != 'A') ||
		(waveFileHeader.format[2] != 'V') || (waveFileHeader.format[3] != 'E'))
	{
		return false;
	}

	// Check that the sub chunk ID is the fmt format.
	if((waveFileHeader.subChunkID[0] != 'f') || (waveFileHeader.subChunkID[1] != 'm') ||
		(waveFileHeader.subChunkID[2] != 't') || (waveFileHeader.subChunkID[3] != ' '))
	{
		return false;
	}

	// Check that the audio format is WAVE_FORMAT_PCM.
	if(waveFileHeader.audioFormat != WAVE_FORMAT_PCM)
	{
		return false;
	}

	// Check that the wave file was recorded in stereo format.
	if(waveFileHeader.numChannels != 2)
	{
		return false;
	}

	// Check that the wave file was recorded at a sample rate of 44.1 KHz.
	if(waveFileHeader.sampleRate != 44100)
	{
		return false;
	}

	// Ensure that the wave file was recorded in 16 bit format.
	if(waveFileHeader.bitsPerSample != 16)
	{
		return false;
	}

	// Check for the data chunk header.
	if((waveFileHeader.dataChunkID[0] != 'd') || (waveFileHeader.dataChunkID[1] != 'a') ||
		(waveFileHeader.dataChunkID[2] != 't') || (waveFileHeader.dataChunkID[3] != 'a'))
	{
		return false;
	}

	// Set the wave format of secondary buffer that this wave file will be loaded onto.
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = 44100;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nChannels = 2;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	// Set the buffer description of the secondary sound buffer that the wave file will be loaded onto.
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME;
	bufferDesc.dwBufferBytes = waveFileHeader.dataSize;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = &waveFormat;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	// Create a temporary sound buffer with the specific buffer settings.
	if(FAILED(m_DirectSound->CreateSoundBuffer(&bufferDesc, &tempBuffer, NULL)))
		return false;

	// Test the buffer format against the direct sound 8 interface and create the secondary buffer.
	if(FAILED(tempBuffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)&*secondaryBuffer)))
		return false;

	// Release the temporary buffer.
	tempBuffer->Release();
	tempBuffer = 0;

	// Move to the beginning of the wave data which starts at the end of the data chunk header.
	fseek(filePtr, sizeof(WaveHeaderType), SEEK_SET);

	// Create a temporary buffer to hold the wave file data.
	waveData = new unsigned char[waveFileHeader.dataSize];
	if(!waveData)
	{
		return false;
	}

	// Read in the wave file data into the newly created buffer.
	if(fread(waveData, 1, waveFileHeader.dataSize, filePtr) != waveFileHeader.dataSize)
		return false;

	// Close the file once done reading.
	if(fclose(filePtr) != 0)
		return false;

	// Lock the secondary buffer to write wave data into it.
	if(FAILED((*secondaryBuffer)->Lock(0, waveFileHeader.dataSize, (void**)&bufferPtr, (DWORD*)&bufferSize, NULL, 0, 0)))
		return false;

	// Copy the wave data into the buffer.
	memcpy(bufferPtr, waveData, waveFileHeader.dataSize);

	// Unlock the secondary buffer after the data has been written to it.
	if(FAILED((*secondaryBuffer)->Unlock((void*)bufferPtr, bufferSize, NULL, 0)))
		return false;

	// Release the wave data since it was copied into the secondary buffer.
	delete [] waveData;
	waveData = 0;

	return true;
}

void D3DAudio::ShutdownWaveFile(IDirectSoundBuffer8** secondaryBuffer)
{
	// Release the secondary sound buffer.
	if(*secondaryBuffer)
	{
		(*secondaryBuffer)->Release();
		*secondaryBuffer = 0;
	}

	return;
}

//Todo Allow for options to be passed as parameters
bool D3DAudio::PlayWaveFile()
{
	// Set position at the beginning of the sound buffer.
	if(FAILED(m_secondaryBuffer1->SetCurrentPosition(0)))
		return false;

	// Set volume of the buffer to 100%.
	if(FAILED(m_secondaryBuffer1->SetVolume(DSBVOLUME_MAX)))
		return false;

	// Play the contents of the secondary sound buffer.
	// The Play() function it will automatically mix the audio into the primary buffer and start it playing if it wasn't already playing.

	// DSBPLAY_LOOPING is the flag to allow for looping, parameter 3
	if(FAILED(m_secondaryBuffer1->Play(0, 0, 0))) // the position is set to the beginning of the secondary sound buffer otherwise it will continue from where it last stopped playing
		return false;

	return true;
}

void D3DAudio::PlayFile( char* fName)
{
	if (!LoadWaveFile(fName, &m_secondaryBuffer1)) // change this to available buffers
	{
		MessageBox(m_hwnd, L"Error Loading Wave File", L"Error", MB_OK);
		//PostQuitMessage( 0 );
	}

	PlayWaveFile(); // change this to play the specified wav file
}