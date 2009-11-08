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
	std::vector<InputDevice>::iterator it;
	BOOL ret = TRUE;
	
	BYTE *buffer = NULL;
	RAWINPUT *raw = (RAWINPUT *)buffer;
	UINT dwSize, dwGetSize;

	GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
	buffer = new BYTE[dwSize];
	if(buffer == NULL)
		return FALSE;
	dwGetSize = GetRawInputData((HRAWINPUT)lParam, RID_INPUT, buffer, &dwSize, sizeof(RAWINPUTHEADER));
	//ASSERT(dwGetSize == dwSize);

	for(it = devices.begin(); it != devices.end(); ++it) {
		if(it->IsBindedToWindow(hWnd) && it->IsDeviceTypeOf(raw->header.dwType)) {
			ret = ret && it->ProcessMessage(hWnd, msg, wParam, lParam);
		}
	}

	delete[] buffer;
	return ret;
}
