#pragma once

#include "Singleton.h"

class Mouse : public Singleton<Mouse> {

    Mouse();
    friend Singleton< Mouse >;
    private:
        static constexpr int KEY_NUM = 8;	//�L�[����
        int _x, _y;
        int _buttonPressingCount[KEY_NUM];//������J�E���^
        int _buttonReleasingCount[KEY_NUM];//������J�E���^
        bool isAvailableCode(int keyCode);//keyCode���L���ȃL�[�ԍ����₤

    public:
        static constexpr int LEFT_CLICK = 0;
        static constexpr int RIGHT_CLICK = 1;
        bool Update();	//�X�V
        int GetPressingCount(int keyCode);//keyCode�̃L�[��������Ă���t���[�������擾
        int GetReleasingCount(int keyCode);//keyCode�̃L�[��������Ă���t���[�������擾
        int getX() { return _x; }
        int getY() { return _y; }
};
