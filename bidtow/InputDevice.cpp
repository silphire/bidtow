#include "InputDevice.h"

InputDevice::InputDevice(void)
{
	hDevice = NULL;
	hBindedWnd = NULL;
}

InputDevice::~InputDevice(void)
{
}

std::string InputDevice::GetName(void)
{
	std::string name;
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

bool InputDevice::IsBindedToWindow(HWND hWnd)
{
	return hWnd != NULL && hWnd == hBindedWnd;
}

bool InputDevice::IsDeviceTypeOf(DWORD type)
{
	return type == deviceType;
}

BOOL InputDevice::ProcessMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return TRUE;
}
