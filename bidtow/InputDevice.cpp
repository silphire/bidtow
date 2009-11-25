#include "InputDevice.h"
#include <cstring>

InputDevice::InputDevice(const RAWINPUT *raw, HWND hWnd)
{
	hBindedWnd = NULL;
	hDevice = NULL;

	Init(raw);
	Bind(hWnd);
}

InputDevice::~InputDevice(void)
{
}

void InputDevice::Init(const RAWINPUT *raw)
{
	UINT size;
	TCHAR *devname, *p, *context;

	hDevice = raw->header.hDevice;
	deviceType = raw->header.dwType;

	GetRawInputDeviceInfo(hDevice, RIDI_DEVICENAME, NULL, &size);
	devname = new TCHAR[size];
	if(GetRawInputDeviceInfo(hDevice, RIDI_DEVICENAME, devname, &size) < 0) {
		delete[] devname;
		return;
	}

	p = _tcstok_s(devname, _T("#"), &context);
	if(p != NULL)
		classCode = p + 4;		// skip "\\??\\"
	p = _tcstok_s(NULL, _T("#"), &context);
	if(p != NULL)
		subClassCode = p;
	p = _tcstok_s(NULL, _T("#"), &context);
	if(p != NULL)
		protocolCode = p;
	p = _tcstok_s(NULL, _T("#"), &context);
	if(p != NULL)
		guid = p;

	delete[] devname;

	// get device description
	ATL::CRegKey key;
	WTL::CString keyName;
	keyName.Format(_T("SYSTEM\\CurrentControlSet\\Enum\\%s\\%s\\%s"), classCode, subClassCode, protocolCode);
	if(key.Open(HKEY_LOCAL_MACHINE, keyName, KEY_READ) == ERROR_SUCCESS) {
		TCHAR *buf = NULL;
		ULONG len, reqlen = 0x100;
		LONG result;
		
		do {
			len = reqlen;
			buf = new TCHAR[len];
			result = key.QueryStringValue(_T("DeviceDesc"), buf, &len);
			if(result == ERROR_MORE_DATA) {
				reqlen *= 2;
				delete[] buf;
			}
		} while(result == ERROR_MORE_DATA);

		if(result == ERROR_SUCCESS) {
			description = buf;
		}

		delete[] buf;
	}
}

WTL::CString InputDevice::GetGUID(void) const
{
	return guid;
}

WTL::CString InputDevice::GetName(void) const
{
	return description;
}

DWORD InputDevice::GetDeviceType(void) const
{
	return deviceType;
}


HWND InputDevice::GetBindedHWND(void) const 
{
	return hBindedWnd;
}

void InputDevice::Bind(HWND hWnd)
{
	if(hWnd == NULL)
		return;

	hBindedWnd = hWnd;
}

void InputDevice::UnBind(void)
{
	if(hBindedWnd == NULL)
		return;
}

bool InputDevice::IsBindedToWindow(HWND hWnd) const
{
	return ::IsWindow(hWnd) && hWnd == hBindedWnd;
}

bool InputDevice::IsDeviceTypeOf(DWORD type) const
{
	return type == deviceType;
}

BOOL InputDevice::ProcessMessage(WPARAM code, RAWINPUT *rawInput)
{
	return TRUE;
}
