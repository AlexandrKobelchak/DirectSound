#pragma once
#include "stdafx.h"

class Observer
{
	Observer(HWND hWnd) : m_hDlg(hWnd) {
	}
	HWND m_hDlg;
	LPDIRECTSOUND m_lpDirectSound;
	LPDIRECTSOUNDCAPTURE m_lpDirectSoundCapture;

public:	
	static INT_PTR CALLBACK MyDlgProc(HWND, UINT, WPARAM, LPARAM);
	static BOOL CALLBACK DSEnumProc(LPGUID, LPCTSTR, LPCTSTR, LPVOID);
private:

	void ChangeSoundDevices();

	BOOL OnInitDialog(HWND, HWND, LPARAM);
	void OnCommand(HWND, int, HWND, UINT);
	void OpenMediaFile();
	void PlayMediaFile(LPTSTR fileName);
	BOOL CreateBasicBuffer(LPDIRECTSOUND lpDirectSound, LPDIRECTSOUNDBUFFER* lplpDsb);
};

