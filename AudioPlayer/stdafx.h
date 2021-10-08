#pragma once

#ifndef _WIN32_WINDOWS    // Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0A00 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE     // Allow use of features specific to IE 6.0 or later.
#define  _WIN32_IE 0x0900 // Change this to the appropriate value to target other versions of IE.
#endif

#define WINVER 0x0A00
#define _WIN32_WINNT 0x0A00


#include <windows.h>
#include <windowsx.h>
#include <CommCtrl.h>
#include <tchar.h>
#include <DSound.h>




#pragma comment (linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma comment (lib, "comctl32.lib")
#pragma comment (lib, "winmm.lib")
#pragma comment(lib, "Dsound.lib")