#pragma once

//
// InputDeviceManager
// �}�E�X��L�[�{�[�h�̊Ǘ�
// �E�ڑ���ؒf (Vista�̏ꍇ�̓��b�Z�[�W�Ŕ��ł���)
// �E����̃f�o�C�X�̃E�B���h�E�ւ̓o�^�菇������
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
