#include "Observer.h"
#include "resource.h"



BOOL CALLBACK Observer::DSEnumProc(
	LPGUID lpGuid,
	LPCTSTR lpszDescr,
	LPCTSTR lpszDrvName,
	LPVOID lpContext) {

	HWND hCombo = (HWND)lpContext;

	if (lpGuid) {
		LPGUID tmp = new GUID(*lpGuid);
		ComboBox_AddString(hCombo, lpszDescr);
		ComboBox_SetItemData(hCombo, ComboBox_FindString(hCombo, 0, lpszDescr), (LPARAM)tmp);
	}

	return TRUE;
}

INT_PTR Observer::MyDlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam) {

	static Observer obs(hDlg);

	switch (msg) {

		HANDLE_MSG(hDlg, WM_COMMAND, obs.OnCommand);
		HANDLE_MSG(hDlg, WM_INITDIALOG, obs.OnInitDialog);
	}

	return FALSE;
}

void Observer::ChangeSoundDevices()
{
	HRESULT hrs = DirectSoundCreate(NULL, &m_lpDirectSound, nullptr);
	HRESULT hrc = DirectSoundCaptureCreate(NULL, &m_lpDirectSoundCapture, nullptr);

	DSCAPS dscaps = { sizeof(DSCAPS) };
	HRESULT hr = m_lpDirectSound->GetCaps(&dscaps);
	m_lpDirectSound->SetCooperativeLevel(m_hDlg, DSSCL_NORMAL);

}

BOOL Observer::OnInitDialog(HWND hDlg, HWND hwndFocus, LPARAM lParam) {

	HWND hComboSoundDev = GetDlgItem(hDlg, IDC_COMBO_DEVICES);
	HWND hComboCaptureDev = GetDlgItem(hDlg, IDC_COMBO_CAPTURE_DEVICES);

	DirectSoundEnumerate((LPDSENUMCALLBACK)DSEnumProc, (LPVOID)hComboSoundDev);

	DirectSoundCaptureEnumerate((LPDSENUMCALLBACK)DSEnumProc, (LPVOID)hComboCaptureDev);

	if (!ComboBox_GetCount(hComboCaptureDev) || !ComboBox_GetCount(hComboSoundDev)) {
		MessageBox(m_hDlg,
			_T("Программа не может работать без устройств захвата и вывода"),
			_T(""),
			MB_OK | MB_ICONERROR);
		EndDialog(m_hDlg, IDCANCEL);
	}
	ComboBox_SetCurSel(hComboCaptureDev, 0);
	ComboBox_SetCurSel(hComboSoundDev, 0);

	return TRUE;
}

void Observer::OnCommand(HWND, int id, HWND, UINT codeNotify)
{
	switch (id)
	{
	case IDC_COMBO_DEVICES:
	case IDC_COMBO_CAPTURE_DEVICES:
		if (codeNotify == CBN_SELCHANGE)
			ChangeSoundDevices();
		break;
	case IDC_BUTTON_OPEN:
		OpenMediaFile();
		return;
	case IDOK:
	case IDCANCEL:
		CoUninitialize();
		EndDialog(m_hDlg, id);
		return;
	}
}

void Observer::OpenMediaFile()
{
	OPENFILENAME ofn = { sizeof(OPENFILENAME) };
	WCHAR fileName[_MAX_PATH] = L"";
	//ofn.hInstance = m_hInstance;
	ofn.hwndOwner = m_hDlg;
	ofn.lpstrFilter = L"All Files\0*.*\0Wav Files(*.wav)\0*.wav\0\0";
	ofn.lpstrTitle = L"Open File";
	ofn.nMaxFile = _MAX_PATH;
	ofn.lpstrFile = fileName;
	ofn.nFilterIndex = 2;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST;
	ofn.lpstrInitialDir = L"%windir%\\media";

	if (GetOpenFileName(&ofn)) {

		//PlayMediaFile(ofn.lpstrFile);
		m_srReader.Open(ofn.lpstrFile);
	}

}

void Observer::PlayMediaFile(LPTSTR fileName)
{
	LPDIRECTSOUND m_lpDirectSound;

	HWND hComboDevices = GetDlgItem(m_hDlg, IDC_COMBO_DEVICES);
	LPGUID lpGuid = (LPGUID)ComboBox_GetItemData(hComboDevices, ComboBox_GetCurSel(hComboDevices));

	HRESULT hrs = DirectSoundCreate(lpGuid, &m_lpDirectSound, nullptr);

	DSCAPS dscaps = { sizeof(DSCAPS) };
	HRESULT hr = m_lpDirectSound->GetCaps(&dscaps);

	m_lpDirectSound->SetCooperativeLevel(m_hDlg, DSSCL_NORMAL);

	LPDIRECTSOUNDBUFFER lpDsb;

	if (CreateBasicBuffer(m_lpDirectSound, &lpDsb)) {

	}
}


BOOL Observer::CreateBasicBuffer(LPDIRECTSOUND lpDirectSound, LPDIRECTSOUNDBUFFER* lplpDsb) {

	PCMWAVEFORMAT pcwf = { sizeof(PCMWAVEFORMAT) };
	pcwf.wf.wFormatTag = WAVE_FORMAT_PCM;
	pcwf.wf.nChannels = 2;//stereo
	pcwf.wf.nSamplesPerSec = 22050;//
	pcwf.wf.nBlockAlign = 4;
	pcwf.wf.nAvgBytesPerSec = pcwf.wf.nSamplesPerSec * pcwf.wf.nBlockAlign;
	pcwf.wBitsPerSample = 16;

	DSBUFFERDESC dsb = { sizeof(DSBUFFERDESC) };
	dsb.dwFlags = DSBCAPS_STATIC;
	dsb.dwBufferBytes = 3 * pcwf.wf.nAvgBytesPerSec;
	dsb.lpwfxFormat = (LPWAVEFORMATEX)&pcwf;

	HRESULT hr = lpDirectSound->CreateSoundBuffer(&dsb, lplpDsb, NULL);

	if (DS_OK != hr) {
		lplpDsb = nullptr;
		return FALSE;
	}

	return TRUE;
}

BOOL Observer::AppCreateWritePrimaryBuffer(
	LPDIRECTSOUND lpDirectSound, 
	LPDIRECTSOUNDBUFFER* lplpDsb, 
	LPDWORD lpdwBufferSize, 
	HWND hWnd) 
{

	//DSBUFFERDESC dsbdesc;
	DSBCAPS dsbcaps;
	HRESULT hr;

	PCMWAVEFORMAT pcmwf;
	memset(&pcmwf, 0, sizeof(PCMWAVEFORMAT));
	pcmwf.wf.wFormatTag = WAVE_FORMAT_PCM;
	pcmwf.wf.nChannels = 2;
	pcmwf.wf.nSamplesPerSec = 22050;//
	pcmwf.wf.nBlockAlign = 4;
	pcmwf.wf.nAvgBytesPerSec = pcmwf.wf.nSamplesPerSec * pcmwf.wf.nBlockAlign;
	pcmwf.wBitsPerSample = 16;

	DSBUFFERDESC dsb = { sizeof(DSBUFFERDESC) };
	dsb.dwFlags = DSBCAPS_PRIMARYBUFFER;
	dsb.dwBufferBytes = 0;
	dsb.lpwfxFormat = NULL;

	hr = lpDirectSound->SetCooperativeLevel(hWnd, DSSCL_WRITEPRIMARY);

	if (DS_OK == hr) {

		hr = lpDirectSound->CreateSoundBuffer(&dsb, lplpDsb, NULL);


		if (DS_OK == hr) {
			hr = (*lplpDsb)->SetFormat((LPCWAVEFORMATEX) &pcmwf);

			if (DS_OK == hr) {
				
				dsbcaps.dwSize = sizeof(DSBCAPS);
				(*lplpDsb)->GetCaps(&dsbcaps);
				*lpdwBufferSize = dsbcaps.dwBufferBytes;
				return TRUE;
			}
		}
	}
	return FALSE;
}


BOOL Observer::AppWriteDataToBuffer(
	LPDIRECTSOUNDBUFFER lpDsb,//pointer to buffer DirectSound
	DWORD dwOffset,           //element position in buffer
	LPBYTE lpbSoundData,      //pointer to sound data
	DWORD dwSoundBytes)       //data size
{
	LPVOID lpPtr_1;
	DWORD dwBytes_1;
	LPVOID lpPtr_2;
	DWORD dwBytes_2;
	HRESULT hr;

	//получение параметров блокируемого фрагмента в аудиобуфере, включающее два указателя
	hr = lpDsb->Lock(dwOffset, dwSoundBytes, &lpPtr_1, &dwBytes_1, &lpPtr_2, &dwBytes_2, 0);

	//если возвращено значение DSERR_BUFFERLOST, пробуем его снова заблокировать
	if (DSERR_BUFFERLOST == hr) {
		lpDsb->Restore();
		hr = lpDsb->Lock(dwOffset, dwSoundBytes, &lpPtr_1, &dwBytes_1, &lpPtr_2, &dwBytes_2, 0);
	}
	
	if (DS_OK == hr) {

		//Запись информации по возвращаемым указателям
		CopyMemory(lpPtr_1, lpbSoundData, dwBytes_1);
		if (NULL != lpPtr_2) {
			CopyMemory(lpPtr_2, lpbSoundData+dwBytes_1, dwBytes_2);
		}
		//Освобождаем фрагмент буфера и передаем егов DirectSound
		hr = lpDsb->Unlock(lpPtr_1, dwBytes_1, lpPtr_2, dwBytes_2);
		if (DS_OK == hr) {
			return TRUE;
		}
	}
	return FALSE;
}
