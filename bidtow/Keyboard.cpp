#include "Keyboard.h"

Keyboard::Keyboard(const RAWINPUT *raw, HWND hWnd) : InputDevice(raw, hWnd)
{
}

Keyboard::~Keyboard(void)
{
}

void Keyboard::Init(const RAWINPUT *raw)
{
}

BOOL Keyboard::ProcessMessage(WPARAM code, RAWINPUT *rawInput)
{
	return InputDevice::ProcessMessage(code, rawInput);
}
