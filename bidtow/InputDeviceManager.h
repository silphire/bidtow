#pragma once

//
// InputDeviceManager
// マウスやキーボードの管理
// ・接続や切断 (Vistaの場合はメッセージで飛んでくる)
// ・特定のデバイスのウィンドウへの登録手順を示す
//

#include <windows.h>
#include <vector>
#include <set>
#include "InputDevice.h"

class InputDeviceManager
{
private:
	std::vector<InputDevice> devices;
public:
	InputDeviceManager(void);
	~InputDeviceManager(void);
	BOOL PassInputMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};
