#pragma once
#include <Windows.h>
#include <mmsystem.h>
#include <mmreg.h>

struct WaveSoundReader
{
	WAVEFORMATEX* m_pWfx;
	HMMIO  m_hMMIOIn;
	MMCKINFO m_ckIn;
	MMCKINFO m_ckInRiff;

	WaveSoundReader();
	~WaveSoundReader();

	HRESULT Open(wchar_t*);
	HRESULT Reset();
	HRESULT Read(UINT, BYTE*, UINT*);
	HRESULT Close();
};

