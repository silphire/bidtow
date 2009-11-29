#pragma once
#include "InputDevice.h"

class Keyboard : public InputDevice
{
public:
	Keyboard(const RAWINPUT *raw, HWND hWnd);
	virtual ~Keyboard(void);
	virtual void Init(const RAWINPUT *raw);
	virtual BOOL ProcessMessage(WPARAM code, const RAWINPUT *rawInput);
};

