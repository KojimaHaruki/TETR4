#include <DxLib.h>
#include "Keyboard.h"

Keyboard::Keyboard() {
    memset(mKeyPressingCount, 0, sizeof(mKeyPressingCount));
    memset(mKeyReleasingCount, 0, sizeof(mKeyReleasingCount));
}

//�X�V
bool Keyboard::Update() {
    char nowKeyStatus[KEY_NUM];
    GetHitKeyStateAll(nowKeyStatus);       //���̃L�[�̓��͏�Ԃ��擾
    for (int i = 0; i < KEY_NUM; i++) {
        if (nowKeyStatus[i] != 0) {            //i�Ԃ̃L�[��������Ă�����
            if (mKeyReleasingCount[i] > 0) {//������J�E���^��0���傫�����
                mKeyReleasingCount[i] = 0;   //0�ɖ߂�
            }
            mKeyPressingCount[i]++;          //������J�E���^�𑝂₷
        }
        else {                             //i�Ԃ̃L�[��������Ă�����
            if (mKeyPressingCount[i] > 0) { //������J�E���^��0���傫�����
                mKeyPressingCount[i] = 0;    //0�ɖ߂�
            }
            mKeyReleasingCount[i]++;         //������J�E���^�𑝂₷
        }
    }
    return true;
}
int Keyboard::GetPressingCount(int KeyCode) {
    if (!Keyboard::IsAvailableCode(KeyCode)) {
        return -1;
    }
    return mKeyPressingCount[KeyCode];
}
int Keyboard::GetReleasingCount(int KeyCode) {
    if (!Keyboard::IsAvailableCode(KeyCode)) {
        return -1;
    }
    return mKeyReleasingCount[KeyCode];
}
bool Keyboard::IsAvailableCode(int KeyCode) {
    if (!(0 <= KeyCode && KeyCode < KEY_NUM)) {
        return false;
    }
    return true;
}