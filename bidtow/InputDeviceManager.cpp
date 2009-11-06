#include "InputDeviceManager.h"
#include <windows.h>

InputDeviceManager::InputDeviceManager(void)
{
}

InputDeviceManager::~InputDeviceManager(void)
{
}

BOOL InputDeviceManager::PassInputMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	RAWINPUT *raw;
	std::vector<InputDevice>::iterator it;
	BOOL ret = TRUE;
	
	for(it = devices.begin(); it != devices.end(); ++it) {
		if(it->IsBindedToWindow(hWnd) && it->IsDeviceTypeOf(raw->header.dwType)) {
			ret = ret && it->ProcessMessage(hWnd, msg, wParam, lParam);
		}
	}

	return ret;
}
