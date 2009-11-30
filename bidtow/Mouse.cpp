#include "Mouse.h"
#include <cmath>

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
	const RAWMOUSE *mouse = &rawInput->data.mouse;
	LONG x = mouse->lLastX;
	LONG y = mouse->lLastY;
	RECT pos;
	HWND hWndTop;

	hWndTop = ::GetWindow(hBindedWnd, GW_HWNDFIRST);
	if(hWndTop == hBindedWnd) {
		// under the active window
		if(::GetWindowRect(hBindedWnd, &pos) != 0) {
			// if outside the window, get back mouse cursor inside
			// (should use ClipCursor() instead?)
			bool force = false;

			if(x < pos.left) {
				x = pos.left;
				force = true;
			} else if(x > pos.right) {
				x = pos.right;
				force = true;
			}

			if(y < pos.top) {
				y = pos.top;
				force = true;
			} else if(y > pos.bottom) {
				y = pos.bottom;
				force = true;
			}

			if(force)
				::SetCursorPos(x, y);

			return TRUE;
		} else {
			return InputDevice::ProcessMessage(code, rawInput);
		}
	} else {
		::SetActiveWindow(hBindedWnd);
		return TRUE;
	}
}
