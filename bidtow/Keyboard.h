#pragma once
#include "InputDevice.h"

class Keyboard : public InputDevice
{
public:
	Keyboard(const RAWINPUT *raw, HWND hWnd);
	virtual ~Keyboard(void);
};

