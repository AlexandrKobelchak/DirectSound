#include "Observer.h"
#include "resource.h"



INT_PTR Observer::MyDlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam) {

	static Observer obs(hDlg);

	switch (msg) {

		HANDLE_MSG(hDlg, WM_COMMAND, obs.OnCommand);
		HANDLE_MSG(hDlg, WM_INITDIALOG, obs.OnInitDialog);
	}

	return FALSE;
}

BOOL Observer::OnInitDialog(HWND hDlg, HWND hwndFocus, LPARAM lParam) {



	return TRUE;
}

void Observer::OnCommand(HWND, int id, HWND, UINT codeNotify)
{
	switch (id)
	{
	
	case IDC_BUTTON_OPEN:
		OpenMediaFile();
		return;
	case IDOK:
	case IDCANCEL:
		
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

		
	}

}

void Observer::PlayMediaFile(LPTSTR fileName)
{
	
}


