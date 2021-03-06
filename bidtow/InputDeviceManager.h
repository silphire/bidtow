#pragma once

//
// InputDeviceManager
// マウスやキーボードの管理
// ・接続や切断 (Vistaの場合はメッセージで飛んでくる)
// ・特定のデバイスのウィンドウへの登録手順を示す
//

#include <vector>
#include <set>
#include "InputDevice.h"
#include "BalloonNotifier.h"

class InputDeviceManager
{
private:
	enum BindingSelectionState {
		NotSelecting, 
		SelectingMouse, 
		SelectingKeyboard, 
	};

	enum BindingSelectionState bindingSelectionState;
	BalloonNotifier *notifier;
	HWND selectingHWND;

public:
	std::vector<InputDevice *> devices;

	InputDeviceManager(void);
	virtual ~InputDeviceManager(void);

	void RegisterNotifier(BalloonNotifier *obj);
	bool InitDevices(HWND hWnd);

	bool CheckDeviceRemoval(void);
	bool CheckWindowClose(void);
	void RemoveDevice(HANDLE hDevice);

	void StartBind(void);
	void SelectWindow(const RAWINPUT *raw);
	void SelectKeyboard(const RAWINPUT *raw);

	BOOL PassInputMessage(WPARAM code, HRAWINPUT hRawInput);
};
