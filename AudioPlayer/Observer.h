#pragma once
#include "stdafx.h"

class Observer
{


	Observer(HWND hWnd) 
		: m_hDlg(hWnd){
	}


	HWND m_hDlg;


public:	
	static INT_PTR CALLBACK MyDlgProc(HWND, UINT, WPARAM, LPARAM);
private:

	
	BOOL OnInitDialog(HWND, HWND, LPARAM);
	void OnCommand(HWND, int, HWND, UINT);
	void OpenMediaFile();
	void PlayMediaFile(LPTSTR fileName);
	
};

