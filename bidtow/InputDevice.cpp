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
	GetRawInputDeviceInfo(hDevice, RIDI_DEVICENAME, &devname, &size);

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
}

WTL::CString InputDevice::GetName(void) const
{
	WTL::CString name;
	char *buf;
	UINT len = 0;

	if(hDevice == NULL)
		return name;

	GetRawInputDeviceInfo(hDevice, RIDI_DEVICENAME, NULL, &len);
	buf = new char[len];
	GetRawInputDeviceInfo(hDevice, RIDI_DEVICENAME, buf, &len);
	name = buf;
	delete[] buf;

	return buf;
}

DWORD InputDevice::GetDeviceType(void) const
{
	return deviceType;
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
