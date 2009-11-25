#pragma once

class BalloonNotifier {
public:
	virtual BOOL ShowBalloon(const TCHAR *msg, const TCHAR *title) const = 0;
	virtual void DisplayBindedInputDevices(void) = 0;
};
