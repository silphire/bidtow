#include "InputDeviceManager.h"
#include "BalloonNotifier.h"
#include <windows.h>

InputDeviceManager::InputDeviceManager(void)
{
}

InputDeviceManager::~InputDeviceManager(void)
{
}

BOOL InputDeviceManager::PassInputMessage(WPARAM code, HRAWINPUT hRawInput)
{
	std::vector<InputDevice>::iterator it;
	BOOL ret = TRUE;
	HWND hWnd;		// handle of window which mouse indicating
	
	BYTE *buffer = NULL;
	RAWINPUT *raw = (RAWINPUT *)buffer;
	UINT dwSize, dwGetSize;

	GetRawInputData(hRawInput, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
	buffer = new BYTE[dwSize];
	if(buffer == NULL)
		return FALSE;
	dwGetSize = GetRawInputData(hRawInput, RID_INPUT, buffer, &dwSize, sizeof(RAWINPUTHEADER));
	//ASSERT(dwGetSize == dwSize);

	for(it = devices.begin(); it != devices.end(); ++it) {
		if(it->IsBindedToWindow(hWnd) && it->IsDeviceTypeOf(raw->header.dwType)) {
			ret = ret && it->ProcessMessage(code, hRawInput);
		}
	}

	delete[] buffer;
	return ret;
}

void InputDeviceManager::StartBind(BalloonNotifier *notifier)
{
	notifier->ShowBalloon(
		_T("Select window using mouse you want to bind with"), 
		_T("bidtow - mouse selection"));
}

void InputDeviceManager::SelectWindow(void)
{
}

void InputDeviceManager::SelectKeyboard(void)
{
}
