#pragma once
#include <windows.h>
#include <string>
#include <mmsystem.h>
#include <mmreg.h>
#include <dsound.h>
#include "WaveSoundReader.h"


struct DSB_Static
{
	LPDIRECTSOUND m_pDS;
	LPDIRECTSOUNDBUFFER m_pDSBuffer;

	DSB_Static(HWND, std::string&);
	virtual ~DSB_Static();

	void SetParameters(int, int, int);
	HRESULT PlayBuffer(WaveSoundReader*, BOOL, DWORD, std::string&);
	void StopBuffer();
};


