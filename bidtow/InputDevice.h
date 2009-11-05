#pragma once
#include <windows.h>
#include <string>

class InputDevice
{
protected:
	HWND hBindedWnd;
	HANDLE hDevice;
	DWORD deviceType;
public:
	InputDevice(void);
	~InputDevice(void);

	std::string GetName();
	void Bind(HWND hWnd);
	void UnBind(void);

	bool IsBindedToWindow(HWND hWnd);
	bool IsDeviceTypeOf(DWORD type);
	BOOL ProcessMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};
