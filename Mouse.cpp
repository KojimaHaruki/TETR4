#include <DxLib.h>
#include "Mouse.h"

Mouse::Mouse() : _buttonPressingCount(), _buttonReleasingCount(), _x(), _y() {
}

//�X�V
bool Mouse::Update() {
    int nowButtonState = GetMouseInput();
    GetMousePoint(&_x, &_y);
    for (int i = 0; i < KEY_NUM; i++) {
        if ((nowButtonState >> i) & 1) {            //i�Ԃ̃{�^����������Ă�����
            if (_buttonReleasingCount[i] > 0) {//������J�E���^��0���傫�����
                _buttonReleasingCount[i] = 0;   //0�ɖ߂�
            }
            _buttonPressingCount[i]++;          //������J�E���^�𑝂₷
        }
        else {                             //i�Ԃ̃L�[��������Ă�����
            if (_buttonPressingCount[i] > 0) { //������J�E���^��0���傫�����
                _buttonPressingCount[i] = 0;    //0�ɖ߂�
            }
            _buttonReleasingCount[i]++;         //������J�E���^�𑝂₷
        }
    }
    return true;
}

//keyCode�̃L�[��������Ă���t���[������Ԃ�
int Mouse::GetPressingCount(int keyCode) {
    if (!isAvailableCode(keyCode)) {
        return -1;
    }
    return _buttonPressingCount[keyCode];
}

//keyCode�̃L�[��������Ă���t���[������Ԃ�
int Mouse::GetReleasingCount(int keyCode) {
    if (!isAvailableCode(keyCode)) {
        return -1;
    }
    return _buttonReleasingCount[keyCode];
}

//keyCode���L���Ȓl���`�F�b�N����
bool Mouse::isAvailableCode(int keyCode) {
    if (!(0 <= keyCode && keyCode < KEY_NUM)) {
        return false;
    }
    return true;
}