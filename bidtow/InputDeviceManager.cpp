#include "InputDeviceManager.h"
#include "BalloonNotifier.h"
#include "Mouse.h"
#include "Keyboard.h"
#include <windows.h>

InputDeviceManager::InputDeviceManager(void)
{
	bindingSelectionState = NotSelecting;
}

InputDeviceManager::~InputDeviceManager(void)
{
}

void InputDeviceManager::RegisterNotifier(BalloonNotifier *obj)
{
	notifier = obj;
}

bool InputDeviceManager::InitDevices(HWND hWnd)
{
	RAWINPUTDEVICE devs[2];

	ZeroMemory(devs, sizeof(devs));
	devs[0].hwndTarget	= devs[1].hwndTarget	= hWnd;
	devs[0].usUsagePage	= devs[1].usUsagePage	= 0x01;
	devs[0].usUsage		= 0x02;	// Usage Page=1, Usage ID=2 -> mouse
	devs[1].usUsage		= 0x06;	// Usage Page=1, Usage ID=6 -> keyboard
	devs[0].dwFlags		= devs[1].dwFlags		= RIDEV_INPUTSINK;

	return RegisterRawInputDevices(devs, 2, sizeof(RAWINPUTDEVICE)) == TRUE;
}

BOOL InputDeviceManager::PassInputMessage(WPARAM code, HRAWINPUT hRawInput)
{
	std::vector<InputDevice *>::const_iterator it;
	BOOL ret = TRUE;
	
	BYTE *buffer = NULL;
	RAWINPUT *raw;
	UINT dwSize, dwGetSize;

	GetRawInputData(hRawInput, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
	buffer = new BYTE[dwSize];
	if(buffer == NULL)
		return FALSE;
	dwGetSize = GetRawInputData(hRawInput, RID_INPUT, buffer, &dwSize, sizeof(RAWINPUTHEADER));
	ATLASSERT(dwGetSize == dwSize);
	raw = (RAWINPUT *)buffer;

	switch(bindingSelectionState) {
		case NotSelecting:
			for(it = devices.begin(); it != devices.end(); ++it) {
				if((*it)->IsDeviceTypeOf(raw->header.dwType)) {
					ret = ret && (*it)->ProcessMessage(code, raw);
				}
			}

			break;
		case SelectingMouse:
			if(raw->header.dwType == RIM_TYPEMOUSE) {
				SelectWindow(raw);
			}
			break;
		case SelectingKeyboard:
			if(raw->header.dwType == RIM_TYPEKEYBOARD) {
				SelectKeyboard(raw);
			}
			break;
	}

	delete[] buffer;
	return ret;
}

bool InputDeviceManager::CheckDeviceRemoval(void)
{
	bool isChanged = false;
	std::vector<InputDevice *>::const_iterator it;
	RAWINPUTDEVICELIST *availDevs;
	UINT i, nDevices;

	GetRawInputDeviceList(NULL, &nDevices, sizeof(RAWINPUTDEVICELIST));
	availDevs = new RAWINPUTDEVICELIST[nDevices];
	GetRawInputDeviceList(availDevs, &nDevices, sizeof(RAWINPUTDEVICELIST));

	for(it = devices.begin(); it != devices.end(); ++it) {
		bool isMissing = true;

		for(i = 0; i < nDevices; ++i) {
			if(availDevs[i].hDevice == (*it)->GetBindedHWND()) {
				isMissing = false;
				break;
			}
		}

		if(isMissing) {
			InputDevice *dev = *it;
			it = devices.erase(it);
			delete dev;
		}
	}

	delete[] availDevs;

	return isChanged;
}

bool InputDeviceManager::CheckWindowClose(void)
{
	bool isChanged = false;
	std::vector<InputDevice *>::const_iterator it;

	for(it = devices.begin(); it != devices.end(); ++it) {
		InputDevice *dev = *it;
		HWND hWnd = dev->GetBindedHWND();
		if(hWnd != NULL && !::IsWindow(hWnd)) {
			it = devices.erase(it);
			delete dev;
			isChanged = true;
		}
	}

	return isChanged;
}

void InputDeviceManager::RemoveDevice(HANDLE hDevice)
{
	std::vector<InputDevice *>::const_iterator it;
	
	for(it = devices.begin(); it != devices.end(); ++it) {
		InputDevice *dev = *it;
		if(hDevice == dev->GetBindedHWND()) {
			it = devices.erase(it);
			delete dev;
		}
	}
}

void InputDeviceManager::StartBind(void)
{
	if(bindingSelectionState == NotSelecting) {
		notifier->ShowBalloon(
			_T("Select window using mouse you want to bind with"), 
			_T("bidtow - mouse selection"));
		bindingSelectionState = SelectingMouse;
	} else {
		notifier->ShowBalloon(
			_T("Previous binding has not completed"), 
			_T("bidtow"));
	}
}

void InputDeviceManager::SelectWindow(const RAWINPUT *raw)
{
	POINT pt;
	HWND hWnd;

	// return unless left button clicked
	if(!(raw->data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN)) 
		return;

	pt.x = raw->data.mouse.lLastX;
	pt.y = raw->data.mouse.lLastY;
	hWnd = WindowFromPoint(pt);
	if(hWnd == NULL) {
		notifier->ShowBalloon(
			_T("Select aborted"), 
			_T("bidtow"));
		bindingSelectionState = NotSelecting;
	} else {
		devices.push_back(new Mouse(raw, hWnd));

		notifier->ShowBalloon(
			_T("Select keyboard to bind to selecting window"), 
			_T("bidtow - keyboard selection"));
		bindingSelectionState = SelectingKeyboard;
		selectingHWND = hWnd;
	}
}

void InputDeviceManager::SelectKeyboard(const RAWINPUT *raw)
{
	if(raw->data.keyboard.VKey == VK_RETURN) {
		devices.push_back(new Keyboard(raw, selectingHWND));
		notifier->ShowBalloon(
			_T("Successfully binded"), 
			_T("bidtow - binding finished"));
	}

	notifier->DisplayBindedInputDevices();
	bindingSelectionState = NotSelecting;
}
