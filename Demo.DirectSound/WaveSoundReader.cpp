#include "WaveSoundReader.h"
#include <sstream>
#include <string>

WaveSoundReader::WaveSoundReader()
	: m_pWfx(nullptr) {

}
WaveSoundReader::~WaveSoundReader() {

	Close();
	delete m_pWfx;
}

HRESULT WaveSoundReader::Open(wchar_t* fileName) {

	delete m_pWfx;

	m_hMMIOIn = mmioOpen(fileName, NULL, MMIO_ALLOCBUF | MMIO_READ);
	if (NULL == m_hMMIOIn) {
		return E_FAIL;
	}

	m_ckInRiff.fccType = mmioFOURCC('W', 'A', 'V', 'E');
	if (mmioDescend(m_hMMIOIn, &m_ckInRiff, NULL, MMIO_FINDRIFF)) {

		mmioClose(m_hMMIOIn, NULL);
		return E_FAIL;
	}

	m_ckIn.ckid = mmioFOURCC('f', 'm', 't', ' ');
	m_ckInRiff.fccType = NULL;
	if (mmioDescend(m_hMMIOIn, &m_ckIn, &m_ckInRiff, MMIO_FINDCHUNK)) {

		mmioClose(m_hMMIOIn, NULL);
		return E_FAIL;
	}
	m_pWfx = (WAVEFORMATEX*) (m_ckIn.cksize < sizeof(WAVEFORMATEX) ?
		 new BYTE[sizeof(WAVEFORMATEX)]{ 0 } : new BYTE[m_ckIn.cksize]{ 0 });

	if (!m_pWfx) {
		mmioClose(m_hMMIOIn, NULL);
		return E_FAIL;
	}

	if (m_ckIn.cksize < sizeof(PCMWAVEFORMAT)) {
		
		mmioClose(m_hMMIOIn, NULL);
		delete m_pWfx;
		m_pWfx = nullptr;
		return E_FAIL;
	}
	else {

		if (mmioRead(m_hMMIOIn, (char*)m_pWfx, m_ckIn.cksize) != m_ckIn.cksize  
			|| m_pWfx->wFormatTag != WAVE_FORMAT_PCM) {
			
			mmioClose(m_hMMIOIn, NULL);
			delete m_pWfx;
			m_pWfx = nullptr;
			return E_FAIL;
		}		
	}

	if (mmioAscend(m_hMMIOIn, &m_ckIn, 0) != MMSYSERR_NOERROR) {
		
		mmioClose(m_hMMIOIn, NULL);
		delete m_pWfx;
		m_pWfx = nullptr;
		return E_FAIL;
	}

	m_ckIn.ckid = mmioFOURCC('d', 'a', 't', 'a');
	m_ckInRiff.fccType = NULL;

	if (mmioDescend(m_hMMIOIn, &m_ckIn, &m_ckInRiff, MMIO_FINDCHUNK)) {
		
		std::wstringstream ss;
		ss << L"ERROR: No data segment in file " << fileName;
		MessageBox(NULL, ss.str().c_str(), L"Error", MB_OK | MB_SYSTEMMODAL);
		mmioClose(m_hMMIOIn, NULL);
		delete m_pWfx;
		m_pWfx = nullptr;
		return E_FAIL;
	}
	return S_OK;
}


HRESULT WaveSoundReader::Reset() { return S_OK; }
HRESULT WaveSoundReader::Read(UINT, BYTE*, UINT*) { return S_OK; }
HRESULT WaveSoundReader::Close() { return S_OK; }
