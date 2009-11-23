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
	enum BindingSelectionState {
		NotSelecting, 
		SelectingMouse, 
		SelectingKeyboard, 
	};

	enum BindingSelectionState bindingSelectionState;
	BalloonNotifier *notifier;
	HWND selectingHWND;
protected:
	std::vector<InputDevice *> devices;

public:
	InputDeviceManager(void);
	virtual ~InputDeviceManager(void);

	void RegisterNotifier(BalloonNotifier *obj);
	bool InitDevices(HWND hWnd);

	void StartBind(void);
	void SelectWindow(const RAWINPUT *raw);
	void SelectKeyboard(const RAWINPUT *raw);

	BOOL PassInputMessage(WPARAM code, HRAWINPUT hRawInput);
};
