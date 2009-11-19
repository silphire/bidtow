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
	std::vector<InputDevice> devices;
public:
	InputDeviceManager(void);
	~InputDeviceManager(void);
	void StartBind(BalloonNotifier *notifier);
	void SelectWindow(void);
	void SelectKeyboard(void);

	BOOL PassInputMessage(WPARAM code, HRAWINPUT hRawInput);
};
