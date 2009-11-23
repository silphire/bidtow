#pragma once
#include <windows.h>

class InputDevice
{
protected:
	HWND hBindedWnd;
	HANDLE hDevice;
	DWORD deviceType;

	WTL::CString classCode;
	WTL::CString subClassCode;
	WTL::CString protocolCode;
	WTL::CString guid;
public:
	InputDevice(const RAWINPUT *raw = NULL, HWND hWnd = NULL);
	virtual ~InputDevice(void);

	WTL::CString GetGUID() const;
	DWORD GetDeviceType(void) const;

	void Bind(HWND hWnd);
	void UnBind(void);

	bool IsBindedToWindow(HWND hWnd) const;
	bool IsDeviceTypeOf(DWORD type) const;
	virtual void Init(const RAWINPUT *raw);
	virtual BOOL ProcessMessage(WPARAM code, RAWINPUT *rawInput);
};
