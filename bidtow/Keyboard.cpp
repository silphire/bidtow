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

BOOL Keyboard::ProcessMessage(WPARAM code, const RAWINPUT *rawInput)
{
	const RAWKEYBOARD *keyboard = &rawInput->data.keyboard;
	return InputDevice::ProcessMessage(code, rawInput);
}
