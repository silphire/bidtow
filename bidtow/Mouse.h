#pragma once
#include "InputDevice.h"
class Mouse : public InputDevice
{
public:
	Mouse(const RAWINPUT *raw, HWND hWnd);
	virtual ~Mouse(void);
	virtual void Init(const RAWINPUT *raw);
	virtual BOOL ProcessMessage(WPARAM code, const RAWINPUT *rawInput);
};

