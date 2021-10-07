#include "DSB_Static.h"


DSB_Static::DSB_Static(HWND hWnd, std::string& strStatus)
{
    HRESULT hr = CoInitialize(NULL);
    
    // Initialization COM
    if ( hr ) {
        strStatus = "Error Initialize COM";
        throw;
    }
    // Create interface IDirectSound
    // for primary audio device
    if (FAILED( hr = DirectSoundCreate(NULL, &m_pDS, NULL))) {

        strStatus = "Error creating interface";
        throw;
    }
    //Set cooperation lavel DSSCL_PRIORITY
    if (FAILED(hr = m_pDS->SetCooperativeLevel(hWnd, DSSCL_PRIORITY))) {
        strStatus = "Error set cooperation lavel";
        throw;
    }

    //Configuration primary buffer
    DSBUFFERDESC dsdb = {sizeof(DSBUFFERDESC)};
    dsdb.dwFlags = DSBCAPS_PRIMARYBUFFER;
    dsdb.dwBufferBytes = 0;
    dsdb.lpwfxFormat = NULL;

    LPDIRECTSOUNDBUFFER pDSBPrimary=nullptr;

    if (FAILED(hr = m_pDS->CreateSoundBuffer(&dsdb, &pDSBPrimary, NULL))) {
        strStatus = "Error creating audiobuffer";
        throw;
    }

    //Set primary buffer format
    WAVEFORMATEX wfx = {0};
    wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.nChannels = 2;
    wfx.nSamplesPerSec = 22050;
    wfx.wBitsPerSample = 16;
    wfx.nBlockAlign = wfx.wBitsPerSample / 8 * wfx.nChannels;
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;

    if (FAILED(hr = pDSBPrimary->SetFormat(&wfx))) {
        strStatus = "Error set audiobuffer format";
        throw;
    }
    if (m_pDSBuffer) {
        pDSBPrimary->Release();
        m_pDSBuffer = nullptr;
    }

}

DSB_Static::~DSB_Static()
{
    if (m_pDSBuffer)
        m_pDSBuffer->Release();
    if (m_pDS)
        m_pDS->Release();
    CoUninitialize();
}


//Set parameters for DirectSound static buffer 
//
void DSB_Static::SetParameters(int nFraquancy, int nPan, int nVolume)
{
    if (m_pDSBuffer) {
        m_pDSBuffer->SetFrequency(nFraquancy);
        m_pDSBuffer->SetPan(nPan);
        m_pDSBuffer->SetVolume(nVolume);
    }
}

HRESULT DSB_Static::PlayBuffer(WaveSoundReader* pSoundReader, BOOL bLooped, DWORD dwCreationFlag, std::string& strStatus)
{
    HRESULT hr;

    // if user may be change input focus before
    // audiodata output, we need  creating new buffer
    // always
    if (m_pDSBuffer) {
        m_pDSBuffer->Release();
        m_pDSBuffer = nullptr;
    }
    BYTE* pbWavData; //pointer to audio block
    UINT cbWaveSize; //Block Size
    //Set Position to begin audio block
    pSoundReader->Reset();
    int nWaveFileSize = pSoundReader->m_ckIn.cksize;

    //allocate memory
    pbWavData = new BYTE[nWaveFileSize];
    if (NULL == pbWavData) {
        return E_OUTOFMEMORY;
    }
    if (FAILED(hr = pSoundReader->Read(nWaveFileSize, pbWavData, &cbWaveSize))) {

        //Non critical error. Need to change status
        strStatus = "Error read from file";
        return S_FALSE;
    }

    //set audiobuffer DMA
    DSBUFFERDESC dsbd = { sizeof(DSBUFFERDESC) };
    dsbd.dwFlags = dwCreationFlag | DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY | DSBCAPS_STATIC ;
    dsbd.dwBufferBytes = cbWaveSize;
    dsbd.lpwfxFormat = pSoundReader->m_pWfx;
    
    //Creating static buffer DirectSound, user is defining focus  
    if (FAILED(hr = m_pDS->CreateSoundBuffer(&dsbd, &m_pDSBuffer, NULL))){
        
        //Non critical error. Need to change status
        strStatus = "Error creating audiobuffer";
        return S_FALSE;
    }
    DWORD dwStatus;
    if(FAILED(hr = m_pDSBuffer->GetStatus(&dwStatus))) {

        //Non critical error. Need to change status
        strStatus = "Error getting status";
        return S_FALSE;
    }
    if (dwStatus & DSBSTATUS_BUFFERLOST) {

        // if application may by activated naw time
        //DirectSound may by not to передать управление
        //поєтому восстановление буфера может закончиться неудачец
        // в єтом случае устанавливантся период ожидания
        //до тех пор, пока DirectSound не передаст управление
        //
        do {
            hr = m_pDSBuffer->Restore();
            if (hr == DSERR_BUFFERLOST) {
                Sleep(10);
            }
        } while (hr = m_pDSBuffer->Restore());
    }
    VOID* pbData_1 = nullptr;
    VOID* pbData_2 = nullptr;
    DWORD dwLength_1;
    DWORD dwLength_2;


    //Buffer blocking;
    if (FAILED(hr = m_pDSBuffer->Lock(0, dsbd.dwBufferBytes, &pbData_1, &dwLength_1, &pbData_2, &dwLength_2, 0L))) {
        //Non critical error. Need to change status
        strStatus = "Error blocing audiobuffer";
        return S_FALSE;
    }

    //Copy info to audiobuffer
    memcpy(pbData_1, pbWavData, dsbd.dwBufferBytes);

    //Unlock buffer because он больше не нужен
    m_pDSBuffer->Unlock(pbData_1, dsbd.dwBufferBytes, NULL, 0);
    pbData_1 = nullptr;
    delete pbWavData;
    //after buffer creating, it is playing
    if (FAILED(hr = m_pDSBuffer->GetStatus(&dwStatus))) {
        return hr;
    }

    if (dwStatus & DSBSTATUS_PLAYING) {
        //need to set Position = 0
        m_pDSBuffer->SetCurrentPosition(0);
    }
    else {
        DWORD dwLooper = bLooped ? DSBPLAY_LOOPING : 0L;
        if (FAILED(hr = m_pDSBuffer->Play(0, 0, dwLooper))){
            return hr;
        }         
    } 
    return S_OK;
}

void DSB_Static::StopBuffer()
{
    if (m_pDSBuffer==nullptr) {
        return;
    }
    m_pDSBuffer->Stop();
    m_pDSBuffer->SetCurrentPosition(0L);
}
