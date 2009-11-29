#include "Mouse.h"

Mouse::Mouse(const RAWINPUT *raw, HWND hWnd) : InputDevice(raw, hWnd)
{
}

Mouse::~Mouse(void)
{
}

void Mouse::Init(const RAWINPUT *raw)
{
}

BOOL Mouse::ProcessMessage(WPARAM code, const RAWINPUT *rawInput)
{
	return InputDevice::ProcessMessage(code, rawInput);
}
