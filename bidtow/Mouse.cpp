#include "Mouse.h"

Mouse::Mouse(const RAWINPUT *raw, HWND hWnd) : InputDevice(raw, hWnd)
{
}

Mouse::~Mouse(void)
{
}
