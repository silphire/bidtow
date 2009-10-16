//
// bidtow - bind input device to one window
//
// bidtow  Copyright (C) 2009  Yuki Mitsui <silphire@gmail.com>
// This program comes with ABSOLUTELY NO WARRANTY.
// This is free software, and you are welcome to redistribute it
// under certain conditions; read LICENSE for details.
//

//
// REFERENCES:
// http://www.codeproject.com/KB/system/rawinput.aspx
//

#include <windows.h>
#include <tchar.h>
#include "resource.h"

#ifndef ASSERT
#if defined(_DEBUG) || defined(DEBUG)
#define STRINGNIZE(x) #x
#define ASSERT(x) \
	if(!(x)) { \
	OutputDebugString(_T("Assertion Failed! at ") _T(__FILE__) _T(STRINGNIZE(__LINE__)) _T("\n")); \
		DebugBreak(); \
	}
#else
#define ASSERT(x)
#endif
#endif

#define NELEMS(x) (sizeof(x) / sizeof(x[0]))

#define WM_NOTIFYREGION (WM_USER + 1)

const TCHAR *appClassName = _T("bidtow");
static UINT s_TaskbarRestartMessage;
static HWND s_hMainDialog;

static BOOL ManipulateIconOnTaskbar(DWORD dwMessage)
{
	HINSTANCE hInstance = GetModuleHandle(NULL);
	NOTIFYICONDATA notifyIcon;

	// append icon to notification region
	ZeroMemory(&notifyIcon, sizeof(notifyIcon));
	notifyIcon.cbSize = sizeof(notifyIcon);
	notifyIcon.hWnd = s_hMainDialog;
	notifyIcon.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	notifyIcon.uCallbackMessage = WM_NOTIFYREGION;
	notifyIcon.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON_BIDTOW));
	_tcscpy_s(notifyIcon.szTip, NELEMS(notifyIcon.szTip), _T("bidtow"));

	while(!Shell_NotifyIcon(dwMessage, &notifyIcon)) {
		// Shell_NotifyIcon() can be timeout, so we should check icon has registered certainly.
		// see also: http://support.microsoft.com/kb/418138/
		DWORD err = GetLastError();
		if(err == ERROR_TIMEOUT) {
			Sleep(100);
			if(Shell_NotifyIcon(NIM_MODIFY, &notifyIcon))
				break;
		} else {
			// something error occured, but not processed specially.
			return FALSE;
		}
	}

	return TRUE;
}

//
//
//
static BOOL AddIconToTaskbar(void)
{
	return ManipulateIconOnTaskbar(NIM_ADD);
}

//
//
//
static BOOL RemoveIconFromTaskbar(void)
{
	return ManipulateIconOnTaskbar(NIM_DELETE);
}

//
//
//
static void TerminateApplication(void)
{
	RemoveIconFromTaskbar();
	DestroyWindow(s_hMainDialog);
	PostQuitMessage(0);
}

//
// process WM_INPUT
//
static BOOL CALLBACK OnInput(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	BYTE *buffer;
	RAWINPUT *raw = (RAWINPUT *)buffer;
	UINT dwSize, dwGetSize;

	GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
	buffer = new BYTE[dwSize];
	if(buffer == NULL)
		return FALSE;
	dwGetSize = GetRawInputData((HRAWINPUT)lParam, RID_INPUT, buffer, &dwSize, sizeof(RAWINPUTHEADER));
	ASSERT(dwGetSize == dwSize);

	if(raw->header.dwType == RIM_TYPEKEYBOARD) {
	} else if(raw->header.dwType == RIM_TYPEMOUSE) {
	}

	delete[] buffer;
	return TRUE;
}

//
// 
//
static BOOL CALLBACK MainDialogProc(HWND hDialog, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HMENU hTrayMenuRoot, hTrayMenu;
	switch(msg) {
		case WM_INITDIALOG: {
			HINSTANCE hInstance = GetModuleHandle(NULL);
			HICON hAppIcon = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(IDI_ICON_BIDTOW), IMAGE_ICON, 32, 32, 0);
			if(hAppIcon) {
				SendMessage(hDialog, WM_SETICON, ICON_SMALL, (LPARAM)hAppIcon);
			}

			hTrayMenuRoot = LoadMenu(hInstance, MAKEINTRESOURCE(IDR_MENU_TASKTRAY));
			hTrayMenu = GetSubMenu(hTrayMenuRoot, 0);

			return TRUE;
		}

		case WM_COMMAND:
			switch(LOWORD(wParam)) {
			case IDOK:
				break;

			case ID_BIND:
				break;

			case ID_CONFIG:
				break;

			case ID_STATUS:
				break;

			case ID_ABOUT:
				break;

			case ID_EXIT:
				TerminateApplication();
				break;
			}
			return TRUE;

		case WM_CLOSE: {
			TerminateApplication();
			return TRUE;
		}

		case WM_NOTIFYREGION:
			if(lParam == WM_LBUTTONDOWN) {
				MessageBox(NULL, _T("Left Button Clicked"), _T("WM_NOTIFYREGION"), MB_OK);
			} else if(lParam == WM_RBUTTONUP) {
				HINSTANCE hInstance = GetModuleHandle(NULL);
				UINT flag = 0;
				POINT pt;

				GetCursorPos(&pt);
				flag |= (pt.x < GetSystemMetrics(SM_CXSCREEN) / 2) ? TPM_LEFTALIGN : TPM_RIGHTALIGN;
				flag |= (pt.y < GetSystemMetrics(SM_CYSCREEN) / 2) ? TPM_TOPALIGN : TPM_BOTTOMALIGN;
				TrackPopupMenu(hTrayMenu, flag, pt.x, pt.y, 0, s_hMainDialog, NULL);
			} else {
				return FALSE;
			}
			return TRUE;

		case WM_INPUT:
			return OnInput(hDialog, msg, wParam, lParam);

		default:
			return FALSE;
	}
}

//
// window procedure for message loop dispatched at WinMain()
//
static LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg) {
		case WM_CREATE:
			s_TaskbarRestartMessage = RegisterWindowMessage(TEXT("TaskbarCreated"));
			break;

		case WM_INPUT:
			break;
		case WM_DESTROY:
			RemoveIconFromTaskbar();
			PostQuitMessage(0);
			break;

		default:
			if(msg == s_TaskbarRestartMessage) {
				// taskbar recreated
				// see also: http://www31.ocn.ne.jp/~yoshio2/vcmemo17-1.html
				AddIconToTaskbar();
			} else
				return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	return 0L;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	WNDCLASSEX wndClass;
	MSG msg;
	RAWINPUTDEVICE devs[2];
	BOOL bResult;
	RAWINPUTDEVICE x;

	// register window class
	ZeroMemory(&wndClass, sizeof(wndClass));
	wndClass.cbSize = sizeof(wndClass);
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.hInstance = hInstance;
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.hIcon = LoadIcon(NULL, MAKEINTRESOURCE(IDI_ICON_BIDTOW));
	wndClass.hCursor = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.lpszClassName = appClassName;
	wndClass.lpfnWndProc = MainWndProc;
	if(!RegisterClassEx(&wndClass)) {
		return 0;
	}

	// create dialog
	s_hMainDialog = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_MAINDIALOG), NULL, MainDialogProc);
	if(!s_hMainDialog)
		return 0;
	ShowWindow(s_hMainDialog, SW_SHOW);
	UpdateWindow(s_hMainDialog);

	if(!AddIconToTaskbar()) {
		CloseWindow(s_hMainDialog);
		return 0;
	}

	// hook keyboard and mouse control
	ZeroMemory(devs, sizeof(devs));
	devs[0].hwndTarget	= devs[1].hwndTarget	= s_hMainDialog;
	devs[0].usUsagePage	= devs[1].usUsagePage	= 0x01;
	devs[0].usUsage		= 0x02;	// Usage Page=1, Usage ID=2 -> mouse
	devs[1].usUsage		= 0x06;	// Usage Page=1, Usage ID=6 -> keyboard
	devs[0].dwFlags		= devs[1].dwFlags		= RIDEV_INPUTSINK;
	bResult = RegisterRawInputDevices(devs, 2, sizeof(RAWINPUTDEVICE));
	if(!bResult) {
		RemoveIconFromTaskbar();
		CloseWindow(s_hMainDialog);
		return 0;
	}

	// message loop
	while(true) {
		BOOL ret = GetMessage(&msg, NULL, 0, 0);
		if(ret == 0 || ret == -1) {
			break;
		} else if(s_hMainDialog == NULL || !IsDialogMessage(s_hMainDialog, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return 0;
}
