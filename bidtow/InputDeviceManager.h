#pragma once

//
// InputDeviceManager
// �}�E�X��L�[�{�[�h�̊Ǘ�
// �E�ڑ���ؒf (Vista�̏ꍇ�̓��b�Z�[�W�Ŕ��ł���)
// �E����̃f�o�C�X�̃E�B���h�E�ւ̓o�^�菇������
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
