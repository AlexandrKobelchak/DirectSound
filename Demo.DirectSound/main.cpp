#include "stdafx.h"
#include "Observer.h"
#include "resource.h"


INT WINAPI _tWinMain(HINSTANCE hInst, HINSTANCE, LPTSTR strCmdLine, INT nCmdShow) {

	INITCOMMONCONTROLSEX initCtrls = { sizeof(INITCOMMONCONTROLSEX) };
	initCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&initCtrls);

	if (DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), NULL, Observer::MyDlgProc) == IDOK) {

	}
	else {

	}
}