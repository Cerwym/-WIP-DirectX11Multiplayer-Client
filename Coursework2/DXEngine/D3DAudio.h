#ifndef D3DAUDIO_H 
#define D3DAUDIO_H

#include <windows.h>
#include <mmsystem.h>
#include <dsound.h>
#include <stdio.h>

#include "Defines.h"

class D3DAudio
{
public:

	D3DAudio();
	D3DAudio(const D3DAudio&){}
	~D3DAudio();

	bool Init(HWND);

	void Cleanup();

	// Plays the specified audio file
	void PlayFile(char*);

private:

	//Todo support for MP3
	struct WaveHeaderType 
	{
		char chunkID[4];
		unsigned long chunkSize;
		char format[4];
		char subChunkID[4];
		unsigned long subChunkSize;
		unsigned short audioFormat;
		unsigned short numChannels;
		unsigned long sampleRate;
		unsigned long bytesPerSecond;
		unsigned short blockAlign;
		unsigned short bitsPerSample;
		char dataChunkID[4];
		unsigned long dataSize;
	};

	bool InitDirectSound(HWND);
	
	bool LoadWaveFile(char*, IDirectSoundBuffer8**);
	void ShutdownWaveFile(IDirectSoundBuffer8**);

	bool PlayWaveFile();

	IDirectSound8* m_DirectSound;
	IDirectSoundBuffer* m_primaryBuffer;

	//Todo allow multiple sounds
	IDirectSoundBuffer8* m_secondaryBuffer1;
	HWND m_hwnd;
};

#endif