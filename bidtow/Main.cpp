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
#include <vector>
#include <map>
#include <atlstr.h>
#include "resource.h"

#include "InputDeviceManager.h"
#include "InputDevice.h"
#include "BalloonNotifier.h"

#define NELEMS(x) (sizeof(x) / sizeof(x[0]))

#define WM_NOTIFYREGION (WM_APP + 1)
#define LIST_INDEX_WINDOW	0
#define LIST_INDEX_MOUSE	1
#define LIST_INDEX_KEYBOARD	2

#define MSG_WM_INPUT_DEVICE_CHANGE(func) \
	if (uMsg == WM_INPUT_DEVICE_CHANGE) \
	{ \
		SetMsgHandled(TRUE); \
		func(wParam, (HANDLE)lParam); \
		lResult = 0; \
		if(IsMsgHandled()) \
			return TRUE; \
	}

#define MSG_WM_NOTIFYREGION(func) \
	if(uMsg == WM_NOTIFYREGION) { \
		SetMsgHandled(TRUE); \
		lResult = 0; \
		func(wParam, lParam); \
		if(IsMsgHandled()) \
			return TRUE; \
	}

#define SYSCOMMAND_ID_HANDLER_EX(id, func) \
	if (uMsg == WM_SYSCOMMAND && id == LOWORD(wParam)) \
	{ \
		SetMsgHandled(TRUE); \
		func((UINT)HIWORD(wParam), (int)LOWORD(wParam), (HWND)lParam); \
		lResult = 0; \
		if(IsMsgHandled()) \
			return TRUE; \
	}

CAppModule _Module;

class CMainDialog : 
	public CDialogImpl<CMainDialog>, 
	public BalloonNotifier {
public:
	enum { IDD = IDD_MAINDIALOG };

	BEGIN_MSG_MAP_EX(CMainDialog)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CLOSE(OnClose)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_INPUT(OnInput)
		MSG_WM_INPUT_DEVICE_CHANGE(OnInputDeviceChange)
		MSG_WM_NOTIFYREGION(OnNotifyRegion)
		MESSAGE_HANDLER_EX(TaskbarRestartMessage, OnTaskbarRestart)
		COMMAND_ID_HANDLER_EX(IDOK, OnOK)
		COMMAND_ID_HANDLER_EX(ID_BIND, OnBind)
		COMMAND_ID_HANDLER_EX(ID_CONFIG, OnConfig)
		COMMAND_ID_HANDLER_EX(ID_STATUS, OnStatus)
		COMMAND_ID_HANDLER_EX(ID_ABOUT, OnAbout)
		COMMAND_ID_HANDLER_EX(ID_EXIT, OnExit)
		SYSCOMMAND_ID_HANDLER_EX(SC_CLOSE, OnSysClose)
	END_MSG_MAP()

	CMainDialog(void);
	virtual ~CMainDialog();
	BOOL ShowBidtowWindow(void);
	BOOL HideBidtowWindow(void);

private:
	static const UINT IconID;		// task tray icon's ID

	UINT TaskbarRestartMessage;
	CMenu TrayMenu;
	InputDeviceManager *theManager;

	BOOL ManipulateIconOnTaskbar(DWORD dwMessage) const;
	BOOL RegisterIconOnTaskbar(DWORD dwMessage, NOTIFYICONDATA *notifyIcon) const;
	BOOL AddIconToTaskbar(void) const;
	BOOL RemoveIconFromTaskbar(void) const;
	BOOL ShowBalloon(const TCHAR *msg, const TCHAR *title) const;
	void DisplayBindedInputDevices(void);

protected:
	CListViewCtrl listCtrl;

	LRESULT OnInitDialog(HWND hWnd, LPARAM lParam);
	void OnClose(void);
	void OnDestroy(void);
	void OnInput(WPARAM code, HRAWINPUT hRawInput);
	void OnInputDeviceChange(WPARAM wParam, HANDLE hDevice);
	void OnNotifyRegion(WPARAM wParam, LPARAM lParam);
	LRESULT OnTaskbarRestart(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnOK(UINT uNotifyCode, int nID, HWND hWndCtrl);
	LRESULT OnBind(UINT uNotifyCode, int nID, HWND hWndCtrl);
	LRESULT OnConfig(UINT uNotifyCode, int nID, HWND hWndCtrl);
	LRESULT OnStatus(UINT uNotifyCode, int nID, HWND hWndCtrl);
	LRESULT OnAbout(UINT uNotifyCode, int nID, HWND hWndCtrl);
	LRESULT OnExit(UINT uNotifyCode, int nID, HWND hWndCtrl);
	LRESULT OnSysClose(UINT uNotifyCode, int nID, HWND hWndCtrl);
};

const UINT CMainDialog::IconID = 1;

const TCHAR *appClassName = _T("bidtow");

struct BindedDevices {
	HWND hWnd; 
	InputDevice *keyboard;
	InputDevice *mouse;
};

CMainDialog::CMainDialog(void)
{
}

CMainDialog::~CMainDialog()
{
}


BOOL CMainDialog::ManipulateIconOnTaskbar(DWORD dwMessage) const
{
	HINSTANCE hInstance = GetModuleHandle(NULL);
	NOTIFYICONDATA notifyIcon;

	// append icon to notification region
	ZeroMemory(&notifyIcon, sizeof(notifyIcon));
	notifyIcon.cbSize = NOTIFYICONDATA_V2_SIZE;
	notifyIcon.hWnd = this->m_hWnd;
	notifyIcon.uID = CMainDialog::IconID;
	notifyIcon.uVersion = NOTIFYICON_VERSION;
	notifyIcon.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	notifyIcon.uCallbackMessage = WM_NOTIFYREGION;
	notifyIcon.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON_BIDTOW));
	_tcscpy_s(notifyIcon.szTip, NELEMS(notifyIcon.szTip), appClassName);

	return RegisterIconOnTaskbar(dwMessage, &notifyIcon);
}

BOOL CMainDialog::RegisterIconOnTaskbar(DWORD dwMessage, NOTIFYICONDATA *notifyIcon) const
{
	while(!Shell_NotifyIcon(dwMessage, notifyIcon)) {
		// Shell_NotifyIcon() can be timeout, so we should check icon has registered certainly.
		// see also: http://support.microsoft.com/kb/418138/
		DWORD err = GetLastError();
		if(err == ERROR_TIMEOUT) {
			Sleep(100);
			if(Shell_NotifyIcon(NIM_MODIFY, notifyIcon))
				break;
		} else {
			// something error occured, but not processed specially.
			return err == ERROR_SUCCESS;
		}
	}

	return TRUE;
}

BOOL CMainDialog::AddIconToTaskbar(void) const
{
	BOOL ret = TRUE;
	ret = ret && ManipulateIconOnTaskbar(NIM_SETVERSION);
	ret = ret && ManipulateIconOnTaskbar(NIM_ADD);
	return ret;
}

BOOL CMainDialog::RemoveIconFromTaskbar(void) const
{
	return ManipulateIconOnTaskbar(NIM_DELETE);
}

BOOL CMainDialog::ShowBalloon(const TCHAR *msg, const TCHAR *title) const
{
	NOTIFYICONDATA notifyIcon;

	::ZeroMemory(&notifyIcon, sizeof(notifyIcon));
	notifyIcon.cbSize = NOTIFYICONDATA_V2_SIZE;
	notifyIcon.hWnd = this->m_hWnd;
	notifyIcon.uID = CMainDialog::IconID;
	notifyIcon.uFlags = NIF_INFO;
	notifyIcon.dwInfoFlags = NIIF_INFO;
	_tcscpy_s(notifyIcon.szInfo, NELEMS(notifyIcon.szInfo), msg);
	_tcscpy_s(notifyIcon.szInfoTitle, NELEMS(notifyIcon.szInfoTitle), title);
	return RegisterIconOnTaskbar(NIM_MODIFY, &notifyIcon);
}

//
//
//
BOOL CMainDialog::ShowBidtowWindow(void)
{
	ShowWindow(SW_SHOW);
	return TRUE;
}

//
//
//
BOOL CMainDialog::HideBidtowWindow(void)
{
	ShowWindow(SW_HIDE);
	return TRUE;
}

//
//
//
void GetCurrentAvailableInputDevicesName()
{
	RAWINPUTDEVICELIST *devices;
	UINT i, nDevice;

	GetRawInputDeviceList(NULL, &nDevice, sizeof(RAWINPUTDEVICELIST));
	devices = new RAWINPUTDEVICELIST[nDevice];
	GetRawInputDeviceList(devices, &nDevice, sizeof(RAWINPUTDEVICELIST));

	for(i = 0; i < nDevice; ++i) {
		if(devices[i].dwType == RIM_TYPEKEYBOARD || devices[i].dwType == RIM_TYPEMOUSE) {
			InputDevice *devobj = new InputDevice();
		}
	}
}

LRESULT CMainDialog::OnInitDialog(HWND hWnd, LPARAM lParam)
{
	CIcon appIcon;
	CRect rect;

	appIcon.LoadIcon(IDI_ICON_BIDTOW);
	this->SetIcon(appIcon);
	if(!AddIconToTaskbar())
		return FALSE;

	TaskbarRestartMessage = RegisterWindowMessage(TEXT("TaskbarCreated"));

	listCtrl = GetDlgItem(IDC_LIST_BINDED);
	listCtrl.InsertColumn(LIST_INDEX_WINDOW, _T("Window"), LVCFMT_LEFT, 100, -1);
	listCtrl.InsertColumn(LIST_INDEX_MOUSE, _T("Mouse"), LVCFMT_LEFT, 100, -1);
	listCtrl.InsertColumn(LIST_INDEX_KEYBOARD, _T("Keyboard"), LVCFMT_LEFT, 100, -1);

	theManager = new InputDeviceManager();
	theManager->RegisterNotifier(this);
	theManager->InitDevices(this->m_hWnd);

	return TRUE;
}


void CMainDialog::OnClose(void)
{
	RemoveIconFromTaskbar();
	DestroyWindow();
	PostQuitMessage(0);
}

void CMainDialog::OnDestroy(void)
{
	RemoveIconFromTaskbar();
	PostQuitMessage(0);
}

void CMainDialog::OnInput(WPARAM code, HRAWINPUT hRawInput)
{
	theManager->PassInputMessage(code, hRawInput);
}

void CMainDialog::OnInputDeviceChange(WPARAM wParam, HANDLE hDevice)
{
}

void CMainDialog::OnNotifyRegion(WPARAM wParam, LPARAM lParam)
{
	if(lParam == WM_LBUTTONDBLCLK) {
		ShowBidtowWindow();
	} else if(lParam == WM_RBUTTONUP) {
		// show context menu
		UINT flag = 0;
		CMenu TrayMenu;
		POINT pt;

		GetCursorPos(&pt);
		flag |= (pt.x < GetSystemMetrics(SM_CXSCREEN) / 2) ? TPM_LEFTALIGN : TPM_RIGHTALIGN;
		flag |= (pt.y < GetSystemMetrics(SM_CYSCREEN) / 2) ? TPM_TOPALIGN : TPM_BOTTOMALIGN;

		TrayMenu.LoadMenu(IDR_MENU_TASKTRAY);
		TrayMenu.GetSubMenu(0).TrackPopupMenu(flag, pt.x, pt.y, this->m_hWnd);
	}
}

LRESULT CMainDialog::OnTaskbarRestart(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// taskbar recreated
	// see also: http://www31.ocn.ne.jp/~yoshio2/vcmemo17-1.html
	AddIconToTaskbar();
	return TRUE;
}

LRESULT CMainDialog::OnOK(UINT uNotifyCode, int nID, HWND hWndCtrl)
{
	return TRUE;
}

//
// start binding window with input device
//
LRESULT CMainDialog::OnBind(UINT uNotifyCode, int nID, HWND hWndCtrl)
{
	theManager->StartBind();
	return TRUE;
}

LRESULT CMainDialog::OnConfig(UINT uNotifyCode, int nID, HWND hWndCtrl)
{
	return TRUE;
}

LRESULT CMainDialog::OnStatus(UINT uNotifyCode, int nID, HWND hWndCtrl)
{
	return TRUE;
}

LRESULT CMainDialog::OnAbout(UINT uNotifyCode, int nID, HWND hWndCtrl)
{
	return TRUE;
}

//
// "Exit" on the tasktray menu
//
LRESULT CMainDialog::OnExit(UINT uNotifyCode, int nID, HWND hWndCtrl)
{
	RemoveIconFromTaskbar();
	DestroyWindow();
	PostQuitMessage(0);
	return TRUE;
}

LRESULT CMainDialog::OnSysClose(UINT uNotifyCode, int nID, HWND hWndCtrl)
{
	HideBidtowWindow();
	return TRUE;
}

void CMainDialog::DisplayBindedInputDevices(void)
{
	int i;
	std::map<HWND, BindedDevices> binded;

	for(std::vector<InputDevice *>::iterator it = theManager->devices.begin(); it != theManager->devices.end(); ++it) {
		HWND hWnd = (*it)->GetBindedHWND();
		WTL::CString name = (*it)->GetName();

		if(binded.find(hWnd) == binded.end()) {
			BindedDevices dev;
			::ZeroMemory(&dev, sizeof(BindedDevices));
			dev.hWnd = hWnd;
			binded.insert(std::pair<HWND, BindedDevices>(hWnd, dev));
		} else {
			if((*it)->GetDeviceType() == RIM_TYPEMOUSE) {
				binded[hWnd].mouse = *it;
			} else if((*it)->GetDeviceType() == RIM_TYPEKEYBOARD) {
				binded[hWnd].keyboard = *it;
			}
		}
	}

	i = 0;
	for(std::map<HWND, BindedDevices>::iterator it = binded.begin(); it != binded.end(); ++it) {
		if(it->first != NULL) {
			ATL::CString title;
			CWindow(it->first).GetWindowText(title);
			listCtrl.AddItem(i, LIST_INDEX_WINDOW, title);
		}
		if(it->second.mouse != NULL)
			listCtrl.AddItem(i, LIST_INDEX_MOUSE, it->second.mouse->GetName());
		if(it->second.keyboard != NULL)
			listCtrl.AddItem(i, LIST_INDEX_KEYBOARD, it->second.keyboard->GetName());
		++i;
	}
}


int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nShowCmd)
{
	CMainDialog dlg;
	CMessageLoop theLoop;

	_Module.Init(NULL, hInstance);
	_Module.AddMessageLoop(&theLoop);

	dlg.Create(NULL);
	dlg.ShowBidtowWindow();

	int ret = theLoop.Run();

	_Module.RemoveMessageLoop();
	_Module.Term();
	return ret;
}
