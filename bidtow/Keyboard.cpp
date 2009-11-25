#include "Keyboard.h"

Keyboard::Keyboard(const RAWINPUT *raw, HWND hWnd) : InputDevice(raw, hWnd)
{
}

Keyboard::~Keyboard(void)
{
}
