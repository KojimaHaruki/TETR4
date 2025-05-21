#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "winmm.lib")   // MSVC —p
#include "Mouse.h"
#include "Keyboard.h"
#include "Tetris.h"
#include "resource.h"
// Initialize various Value
Tetris::Tetris() : Scene(ConfigID::Home), PlayingBGMNo(BGMID::TETRIS), BlockBox(), BlockAngle(0), BlockShape(), 
WindowMode(FALSE), CharNo(CharID::Initial), CharLv(0), Mode(ModeID::Initial), Pause(PauseID::Resume), MuteMode(MuteModeID::Unmute), KeyImage() {
    SetWindowIconID(IDI_ICON1); SetWindowText("TETR4"); SetLogDrawOutFlag(FALSE); if (DxLib_Init()) exit(-1);
    SRand(GetNowCount()); // initialize random number by the lapse time in Windows
    ChangeWindow(WindowMode);
    // load color
    TitleColor = c.CreateColorToHandle(c.SkyGray); FrameColor = c.CreateColorToHandle(c.CocoaBrown);
    StrColor = c.CreateColorToHandle(c.W);         HPColor = c.CreateColorToHandle(c.CherryPink);
    BlockColor[MinoID::T] = c.CreateColorToHandle(c.VIO); BlockColor[MinoID::O] = c.CreateColorToHandle(c.Y);
    BlockColor[MinoID::S] = c.CreateColorToHandle(c.G);   BlockColor[MinoID::Z] = c.CreateColorToHandle(c.Strawberry);
    BlockColor[MinoID::J] = c.CreateColorToHandle(c.COB); BlockColor[MinoID::L] = c.CreateColorToHandle(c.Marigold);
    BlockColor[MinoID::I] = c.CreateColorToHandle(c.C);   BlockColor[MinoID::Space] = c.CreateColorToHandle(c.BK);
    // load sound
    for (int i = 0; i < BGMID::Num; i++) BGM[i] = LoadSoundMemByResource(MAKEINTRESOURCE(IDR_WAVE1 + i), "MP3");
    for (int i = 0; i < SEID::Num; i++) SE[i] = LoadSoundMemByResource(MAKEINTRESOURCE(IDR_WAVE10 + i), "MP3");
    for (int i = 0; i < SEID::ComboNum; i++) ComboSE[i] = LoadSoundMemByResource(MAKEINTRESOURCE(IDR_WAVE14 + i), "MP3");
    // set character's BGM
    CharBGM[CharID::John]    = BGMID::DeadAlive; CharBGM[CharID::Komachi] = BGMID::NothingDies; 
    CharBGM[CharID::Misuzu]  = BGMID::Chocolate; CharBGM[CharID::Michel]  = BGMID::Hometown; 
    CharBGM[CharID::Yoichi]  = BGMID::Legend;    CharBGM[CharID::Moscow]  = BGMID::TETRIS;
    for (int i = 0; i < MinoID::BagNum; i++) MinoBag[i] = i % MinoID::TypeNum; // set mino bag
    FILE* icdf = fopen(DataFileName, "rb"); // open data file
    if (icdf == NULL) { // if file doesn't exist, 
        InitScene(); // initialize
    }
    else { // if file exists,
        fread(&cd, sizeof(cd), 1, icdf); fclose(icdf); // read & close file
        ChangeSoundVol(0, cd.SoundVol[0]); // set sound's volume
    }
    // set unchangeable key
    cd.Key[CtrlID::Start] = KEY_INPUT_SPACE; cd.Key[CtrlID::Yes] = KEY_INPUT_RETURN; cd.Key[CtrlID::No] = KEY_INPUT_BACK;
    UpdateScene(SceneID::Home); // set home scene
}
void Tetris::ChangeWindow(int WindowModeFlag) {
    ChangeWindowMode(WindowModeFlag); SetDrawScreen(DX_SCREEN_BACK);
    // load image
    for (int i = 0; i < KeyNum; i++) KeyImage[KeyNo[i]] = LoadGraphToResource(MAKEINTRESOURCE(KeyNo[i]), "PNG");
    for (int i = 0; i < CharID::Num; i++) CharImage[i] = LoadGraphToResource(MAKEINTRESOURCE(IDB_PNG68 + i), "PNG");
    for (int i = 0; i < IconID::Num - WindowModeFlag; i++) Icon[i] = LoadGraphToResource(MAKEINTRESOURCE(IDB_PNG74 + i), "PNG");
    // load font
    TitleFont = CreateFontToHandle("Arial Rounded MT Bold", 60, 5, DX_FONTTYPE_ANTIALIASING_EDGE);
    NormalFont = CreateFontToHandle("Calisto MT Bold", 18, 5, DX_FONTTYPE_NORMAL);
    CharNameFont = CreateFontToHandle("Arial Rounded MT Bold", 30, 2, DX_FONTTYPE_ANTIALIASING_EDGE);
    WindowMode = GetWindowModeFlag(); return;
}
void Tetris::Config() {
    DrawBox(0, 25, 640, 455, BlockColor[2], TRUE); DrawBox(0, 25, 412, 325, BlockColor[0], TRUE);
    DrawStringToHandle(5, 25, "Music", BlockColor[7], TitleFont); 
    DrawStringToHandle(412, 25, "System", StrColor, TitleFont);
    DrawStringToHandle(5, 180, "Play Style", BlockColor[7], NormalFont);
    DrawStringToHandle(115, 180, (": " + BGMModeName[cd.BGMMode]).c_str(), BlockColor[7], NormalFont);
    DrawStringToHandle(0, 210, "Scene Theme", BlockColor[7], NormalFont);
    for (int i = 0; i < 2; i++) {
        ChangeKey(349 + 31 * i, 53, 380 + 31 * i, 83, CtrlID::Down + i);
        DrawGraph(352 + 31 * i, 55, KeyImage[cd.Key[CtrlID::Down + i]], TRUE);
        DrawGraph(355 + 25 * i, 175, Icon[IconID::Down + i], TRUE); 
        DrawGraph(420 + 110 * i, 395, Icon[IconID::Hold + 14 * i], TRUE);
        ChangeKey(497 + 110 * i, 393, 528 + 110 * i, 423, CtrlID::Hold + 14 * i);
        DrawGraph(500 + 110 * i, 395, KeyImage[cd.Key[CtrlID::Hold + 14 * i]], TRUE);
    } DrawStringToHandle(450, 400, "Hold             Back", StrColor, NormalFont);
    ChangeKey(607, 423, 638, 453, CtrlID::Forward); DrawGraph(610, 425, KeyImage[cd.Key[CtrlID::Forward]], TRUE);
    DrawStringToHandle(450, 430, "Forward", StrColor, NormalFont); DrawGraph(420, 425, Icon[IconID::Forward], TRUE); 
    for (int i = 0; i < SoundID::Num; i++) {
        DrawStringToHandle(0, 90 + 30 * i, SoundName[i].c_str(), BlockColor[7], NormalFont);
        DrawBox(100, 95 + 30 * i, 100 + cd.SoundVol[i], 105 + 30 * i, StrColor, TRUE);
        DrawBox(100 + cd.SoundVol[i], 95 + 30 * i, 300, 105 + 30 * i, BlockColor[7], TRUE);
        DrawBox(95 + cd.SoundVol[i], 90 + 30 * i, 105 + cd.SoundVol[i], 110 + 30 * i, BlockColor[2], TRUE);
        DrawFormatStringToHandle(315, 90 + 30 * i, BlockColor[7], NormalFont, "%3d", cd.SoundVol[i]);
        DrawStringToHandle(5, 240 + 30 * i, SceneName[i].c_str(), BlockColor[7], NormalFont);
        DrawStringToHandle(115, 240 + 30 * i, (": " + BGMName[cd.SceneBGM[i]]).c_str(), BlockColor[7], NormalFont);
        for (int j = 0; j < 2; j++) { 
            for (int k = 0; k < 2; k++) { DrawGraph(355 + 25 * j, 85 + 30 * i + 150 * k, Icon[IconID::Down + j], TRUE); } 
        }
        if (ClickBox(355, 85 + 30 * i, 380, 110 + 30 * i) > 10) ChangeSoundVol(i, cd.SoundVol[i] - 1);
        else if (ClickBox(380, 85 + 30 * i, 405, 110 + 30 * i) > 10) ChangeSoundVol(i, cd.SoundVol[i] + 1);
        else if (ClickBox(355, 85 + 30 * i, 380, 110 + 30 * i) == 1) ChangeSoundVol(i, cd.SoundVol[i] - 4);
        else if (ClickBox(380, 85 + 30 * i, 405, 110 + 30 * i) == 1) ChangeSoundVol(i, cd.SoundVol[i] + 4);
        if (ClickBox(99, 85 + 30 * i, 301, 115 + 30 * i) > 0) ChangeSoundVol(i, CursorX - 100);
        if (ClickBox(355, 235 + 30 * i, 405, 260 + 30 * i) == 1) { PlaySoundMem(SE[1], DX_PLAYTYPE_BACK);
            if (CursorX < 380) { cd.SceneBGM[i] += BGMID::Num - 1; } else { cd.SceneBGM[i]++; } cd.SceneBGM[i] %= BGMID::Num;
        }
    } 
    for (int i = 0; i < 7; i++) {
        DrawGraph(420, 95 + 30 * i, Icon[IconID::Home + i], TRUE); 
        DrawStringToHandle(450, 100 + 30 * i, CtrlName[IconID::Home + i].c_str(), StrColor, NormalFont);
    } 
    DrawGraph(420, 305, Icon[IconID::AnotherWindow + WindowMode], TRUE); 
    DrawStringToHandle(450, 310, CtrlName[CtrlID::ChWindow].c_str(), StrColor, NormalFont);
    DrawGraph(420, 335, Icon[IconID::Mute + MuteMode + 1], TRUE); 
    DrawStringToHandle(450, 340, MuteName[MuteMode].c_str(), StrColor, NormalFont);
    DrawGraph(420, 365, Icon[IconID::Pause + Pause + 2], TRUE);
    DrawStringToHandle(450, 370, PauseName[Pause].c_str(), StrColor, NormalFont);
    for (int i = 0; i < 4; i++) {
        DrawStringToHandle(5, 340 + 30 * i, BoardCtrlName[i].c_str(), StrColor, NormalFont);
        DrawStringToHandle(56, 340 + 30 * i, (":      " + CtrlName[2 * i]).c_str(), StrColor, NormalFont);
        DrawStringToHandle(287, 340 + 30 * i, CtrlName[1 + 2 * i].c_str(), StrColor, NormalFont);
        for (int j = 0; j < 2; j++) {
            ChangeKey(224 + 160 * j, 333 + 30 * i, 255 + 160 * j, 363 + 30 * i, j + 2 * i);
            DrawGraph(227 + 160 * j, 335 + 30 * i, KeyImage[cd.Key[j + 2 * i]], TRUE);
            DrawGraph(64 + 194 * j, 335 + 30 * i, Icon[j + 2 * i], TRUE);
        }
    }
    if (GhostMode) DrawStringToHandle(190, 430, "On", StrColor, NormalFont);
    else DrawStringToHandle(190, 430, "Off", StrColor, NormalFont);
    if (SkillMode) DrawStringToHandle(350, 430, "On", StrColor, NormalFont);
    else DrawStringToHandle(350, 430, "Off", StrColor, NormalFont);
    for (int i = 0; i < 10; i++) {
        ChangeKey(607, 93 + 30 * i, 638, 123 + 30 * i, CtrlID::Home + i); 
        DrawGraph(610, 95 + 30 * i, KeyImage[cd.Key[CtrlID::Home + i]], TRUE);
    }
    if (HitCtrlKey(CtrlID::Back) == 1 || ClickBox(30, 0, 55, 25) == 1) UpdateScene(Scene + SceneID::Num);
    else if (ClickBox(355, 175, 405, 200) == 1) { PlaySoundMem(SE[1], DX_PLAYTYPE_BACK);
        if (CursorX < 380) { cd.BGMMode += BGMModeID::Num - 1; } else { cd.BGMMode++; } cd.BGMMode %= BGMModeID::Num;
        if (!cd.BGMMode) { PlaySoundMem(BGM[PlayingBGMNo], DX_PLAYTYPE_BACK | DX_PLAYTYPE_LOOP); }
        else { PlaySoundMem(BGM[PlayingBGMNo], DX_PLAYTYPE_BACK); }
    } return;
}
int Tetris::HitKey(int KeyCode) { 
    switch (KeyCode) {
    case KEY_INPUT_LSHIFT: case KEY_INPUT_RSHIFT: 
        return Keyboard::GetInstance()->GetPressingCount(KEY_INPUT_LSHIFT) 
             + Keyboard::GetInstance()->GetPressingCount(KEY_INPUT_RSHIFT);
    case KEY_INPUT_LCONTROL: case KEY_INPUT_RCONTROL: 
        return Keyboard::GetInstance()->GetPressingCount(KEY_INPUT_LCONTROL) 
             + Keyboard::GetInstance()->GetPressingCount(KEY_INPUT_RCONTROL);
    case KEY_INPUT_LALT: case KEY_INPUT_RALT: 
        return Keyboard::GetInstance()->GetPressingCount(KEY_INPUT_LALT) 
             + Keyboard::GetInstance()->GetPressingCount(KEY_INPUT_RALT);
    default:
        return Keyboard::GetInstance()->GetPressingCount(KeyCode);
    }
}
int Tetris::HitCtrlKey(int CtrlNo) { return HitKey(cd.Key[CtrlNo]); }
bool Tetris::ChangeKey(int x1, int y1, int x2, int y2, int CtrlNo) {
    if (CursorX > x1 && CursorY > y1 && CursorX < x2 && CursorY < y2) { // if cursor is in area that key is changeable
        DrawBox(x1, y1, x2, y2, BlockColor[4], TRUE); // notify user that key is changeable
        for (int i = 0; i < KeyNum; i++) {
            if (HitKey(KeyNo[i]) > 0) { // if key that user wants to set is pressed
                for (int j = 0; j < CtrlID::Num; j++) { 
                    if (KeyNo[i] == cd.Key[j]) { // if key is already used for the other control
                        DrawBox(x1, y1, x2, y2, BlockColor[3], TRUE); // notify user that key can't be changed
                        return false;
                    } 
                } // if key isn't used for the other controls
                cd.Key[CtrlNo] = KeyNo[i]; // set key
                PlaySoundMem(SE[1], DX_PLAYTYPE_BACK); // notify user that key is changed
                return true;
            }
        }
    } return false;
}
bool Tetris::ChangeSound(int MuteModeNo) {
    if (MuteModeNo < 0 || MuteModeNo >= MuteModeID::Num) return false; // if mute mode doesn't exist, exit
    if (MuteModeNo == MuteModeID::Mute) StopSoundMem(BGM[PlayingBGMNo]); // if mute is selected, stop BGM
    else { // if unmute is selected
        ChangeVolumeSoundMem(BGMVol, BGM[PlayingBGMNo]); // set BGM volume
        // play BGM
        if (cd.BGMMode == BGMModeID::Loop) { PlaySoundMem(BGM[PlayingBGMNo], DX_PLAYTYPE_BACK | DX_PLAYTYPE_LOOP, 0); }
        else { PlaySoundMem(BGM[PlayingBGMNo], DX_PLAYTYPE_BACK, 0); } 
    } MuteMode = CheckSoundMem(BGM[PlayingBGMNo]); // check mute mode
    return true;
}
bool Tetris::ChangeSoundVol(int SoundNo, int Vol) {
    if (SoundNo < 0 || SoundNo >= SoundID::Num) return false; // if sound doesn't exist, exit
    if (Vol > SoundVol::Max) { // if sound's volume is above maximum volume,
        cd.SoundVol[SoundNo] = SoundVol::Max; // set maximum volume
    }
    else if (Vol < SoundVol::Min) { // if sound's volume is below minimum volume,
        cd.SoundVol[SoundNo] = SoundVol::Min; // set minimum volume
    }
    else cd.SoundVol[SoundNo] = Vol; // othewise set sound's volume
    if (SoundNo != SoundID::SE) { // if sound is BGMs or total sounds,
        BGMVol = (int)(0.0064 * cd.SoundVol[SoundID::Total] * cd.SoundVol[SoundID::BGM]); // set BGMs' volume
        ChangeVolumeSoundMem(BGMVol, BGM[PlayingBGMNo]); // change volume of BGM which is now playing
    }
    if (SoundNo != SoundID::BGM) { // if sound is SEs or total sounds,
        SEVol = (int)(0.0064 * cd.SoundVol[SoundID::Total] * cd.SoundVol[SoundID::SE]); // set SEs' volume
        // change SEs'volume
        for (int i = 0; i < SEID::Num; i++) { ChangeVolumeSoundMem(SEVol, SE[i]); }
        for (int i = 0; i < SEID::ComboNum; i++) { ChangeVolumeSoundMem(SEVol, ComboSE[i]); }
    } return true;
}
bool Tetris::PlayBGM(int BGMNo) {
    if (BGMNo < 0 || BGMNo >= BGMID::Num) return false; // if BGM doesn't exist, exit
    // if BGM exists,
    StopSoundMem(BGM[PlayingBGMNo]);  // stop BGM which is now playing
    ChangeVolumeSoundMem(BGMVol, BGM[BGMNo]); // set BGM's volume
    if (MuteMode == MuteModeID::Unmute) { // if sounds aren't muted, 
        // play BGM
        if (cd.BGMMode == BGMModeID::Loop) { PlaySoundMem(BGM[BGMNo], DX_PLAYTYPE_BACK | DX_PLAYTYPE_LOOP); }
        else { PlaySoundMem(BGM[BGMNo], DX_PLAYTYPE_BACK); } 
    } 
    PlayingBGMNo = BGMNo; // set BGM
    return true;
}
int Tetris::ClickBox(int x1, int y1, int x2, int y2) {
    if (CursorX > x1 && CursorY > y1 && CursorX < x2 && CursorY < y2) // if the cursor is in the area,
        return Mouse::GetInstance()->GetPressingCount(Mouse::LEFT_CLICK);
    return -1; // if the cursor isn't in the area, exit
}
void Tetris::Home() {
    DrawBox(0, 25, 640, 455, FrameColor, TRUE);
    for (int i = 0; i < 7; i++) {
        for (int j = 0; j < 5; j++) {
            DrawBox(85 * i + 24, 85 * j + 29, 85 * i + 106, 85 * j + 111, BlockColor[BlockBox[i][j]], TRUE);
        }
    }
    for (int i = 0; i < 4; i++) { DrawGraph(160 * i - 50, 30, CharImage[i], TRUE); }
    DrawStringToHandle(220, 210, "TETR4", TitleColor, TitleFont); 
    DrawGraph(265, 387, KeyImage[cd.Key[CtrlID::Start]], TRUE);
    DrawStringToHandle(295, 390, "space", StrColor, NormalFont);
    DrawStringToHandle(210, 390, "Press                         to start...", c.GetColorHandle(c.BK), NormalFont);
    if (HitCtrlKey(CtrlID::Start) == 1) UpdateScene(SceneID::ModeSelect); return;
}
void Tetris::ModeSelect() {
    if (Mode >= -ModeID::Num) DrawGraph(55, 0, Icon[CtrlID::Forward], TRUE);
    if (HitCtrlKey(CtrlID::Back) == 1 || ClickBox(30, 0, 55, 25) == 1) { UpdateScene(SceneID::Home); return; }
    if (Mode >= -ModeID::Num && (HitCtrlKey(CtrlID::Forward) == 1 || ClickBox(55, 0, 80, 25) == 1)) Mode += ModeID::Num;
    if (ClickBox(565, 430, 640, 455) == 1 || HitCtrlKey(CtrlID::Skip) == 1) Mode += ModeID::Num; 
    if (Mode == ModeID::Skip) Mode = ModeID::Battle;
    if (Mode >= ModeID::Shuttle && Mode <= ModeID::Battle) { UpdateScene(SceneID::CharSelect); return; }
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            DrawBox(320 * i, 25 + 215 * j, 320 + 320 * i, 240 + 215 * j, BlockColor[i + 2 * j], TRUE);
            DrawGraph(320 * i - 30, 25 + 230 * j, CharImage[i + 2 * j], TRUE);
            DrawStringToHandle(320 * i, 120 + 220 * j, ModeName[i + 2 * j].c_str(), StrColor, TitleFont);
            if (ClickBox(320 * i, 25 + 215 * j, 320 + 320 * i, 240 + 215 * j) == 1) {
                Mode = i + 2 * j; UpdateScene(SceneID::CharSelect); return;
            }
        }
    } DrawBox(0, 230, 640, 250, BlockColor[4], TRUE);
    DrawStringToHandle(150, 180, "Deal with\n    higher speed", BlockColor[7], NormalFont);
    DrawStringToHandle(480, 180, "\nDelete 150 lines", StrColor, NormalFont);
    DrawStringToHandle(150, 400, "Delete 40 lines\n                  speedy", StrColor, NormalFont);
    DrawStringToHandle(480, 400, "Beat opponent", StrColor, NormalFont);
    DrawStringToHandle(220, 230, "Select play mode...", StrColor, NormalFont); return;
}
void Tetris::CharSelect() {
    if (CharNo >= -CharID::Num) DrawGraph(55, 0, Icon[IconID::Forward], TRUE);
    for (int i = 0; i <= CharID::Michel; i++) {
        DrawBox(160 * i, 25, 160 * (i + 1), 235, BlockColor[i], TRUE); DrawGraph(160 * i - 50, 30, CharImage[i], TRUE);
        DrawStringToHandle(160 * i + 15, 200, CharName[i].c_str(), StrColor, CharNameFont);
        if (ClickBox(160 * i, 25, 160 * (i + 1), 235) == 1) { if (CharNo != i) { PlayBGM(CharBGM[i]); } CharNo = i; }
        if (CharNo == i) DrawGraph(160 * i, 60, Icon[IconID::Selected], TRUE);
    } 
    for (int i = 0; i < 2; i++) { 
        DrawBox(160 * i, 235, 160 * (i + 1), 455, BlockColor[CharID::Yoichi + i], TRUE); 
        DrawGraph(160 * i - 50, 240, CharImage[CharID::Yoichi + i], TRUE);
        DrawStringToHandle(15 + 160 * i, 410, CharName[CharID::Yoichi + i].c_str(), StrColor, CharNameFont);
        if (ClickBox(160 * i, 235, 160 * (i + 1), 455) == 1) { 
            if (CharNo != CharID::Yoichi + i) { PlayBGM(CharBGM[CharID::Yoichi + i]); } CharNo = CharID::Yoichi + i; }
        if (CharNo == CharID::Yoichi + i) DrawGraph(160 * i, 270, Icon[IconID::Selected], TRUE);
    } DrawBox(320, 235, 480, 455, TitleColor, TRUE); 
    DrawFormatStringToHandle(427, 245, c.GetColorHandle(c.BK), NormalFont, "Lv.%d", CharLv + 1);
    if (ClickBox(565, 430, 640, 455) == 1 || HitCtrlKey(CtrlID::Skip) == 1) { 
        DrawBox(480, 235, 640, 455, c.GetColorHandle(c.COB), TRUE);
        if (CharNo < 0) { CharNo += CharID::Num; } if (CharNo == CharID::Skip) { CharNo = CharID::Moscow; } 
        UpdateScene(SceneID::GameStart);
    }
    else if (CharNo < 0) { DrawBox(480, 235, 640, 455, c.GetColorHandle(c.COB), TRUE);
        DrawStringToHandle(485, 260, "Select\n    an opponent...", StrColor, NormalFont);
        if (HitCtrlKey(CtrlID::Back) == 1 || ClickBox(30, 0, 55, 25) == 1) { UpdateScene(SceneID::ModeSelect); }
        else if (CharNo >= -CharID::Num && (HitCtrlKey(CtrlID::Forward) == 1 || ClickBox(55, 0, 80, 25) == 1)) {
            CharNo += CharID::Num; PlayBGM(CharBGM[CharNo]); }
    }
    else { DrawGraph(270, 240, CharImage[CharNo], TRUE); DrawBox(480, 235, 640, 455, c.GetColorHandle(c.COB), TRUE);
        for (int i = 0; i < 2; i++) { DrawGraph(605, 340 + 55 * i, KeyImage[cd.Key[CtrlID::Yes + i]], TRUE); }
        DrawStringToHandle(320, 245, (CharRole[CharNo]).c_str(), c.GetColorHandle(c.BK), NormalFont);
        DrawStringToHandle(485, 245, CharFeature[CharNo].c_str(), StrColor, NormalFont);
        DrawStringToHandle(485, 360, "Game Start!!", StrColor, NormalFont);
        DrawStringToHandle(485, 400, "Cancel...", StrColor, NormalFont); DrawGraph(55, 0, Icon[IconID::Forward], TRUE);
        DrawStringToHandle(335, 410, CharName[CharNo].c_str(), StrColor, CharNameFont);
        DrawFormatStringToHandle(485, 320, StrColor, NormalFont, "Your Max Lv.%d", cd.MaxCharLv[Mode][CharNo] + 1);
        if (ClickBox(320, 235, 480, 455) == 1) { CharLv = (CharLv + 1) % (cd.MaxCharLv[Mode][CharNo] + 1); }
        if (HitCtrlKey(CtrlID::No) == 1 || HitCtrlKey(CtrlID::Back) == 1 || ClickBox(30, 0, 55, 25) == 1) {
            CharNo -= CharID::Num; PlayBGM(cd.SceneBGM[SceneID::CharSelect]); }
        else if (HitCtrlKey(CtrlID::Forward) == 1 || ClickBox(55, 0, 80, 25) == 1 ||
            HitCtrlKey(CtrlID::Yes) == 1 || ClickBox(55, 0, 80, 25) == 1) { UpdateScene(SceneID::GameStart); }
    } return;
}
void Tetris::GameStart() {
    DrawBox(0, 25, 640, 455, BlockColor[CharNo], TRUE); DrawGraph(260, 40, CharImage[CharNo], TRUE);
    DrawGraph(55, 0, Icon[IconID::Forward], TRUE); 
    DrawStringToHandle(100, 300, ("V.S.   " + CharName[CharNo]).c_str(), StrColor, TitleFont);
    t.Update();
    DrawFormatStringToHandle(100, 400, StrColor, NormalFont, "Game start in %d s without control", StartCount / 1000 - t.GetLapseTime().s);
    if (HitCtrlKey(CtrlID::Back) == 1 || ClickBox(30, 0, 55, 25) == 1) UpdateScene(SceneID::CharSelect);
    if (HitCtrlKey(CtrlID::Forward) == 1 || ClickBox(55, 0, 80, 25) == 1 || 
        HitCtrlKey(CtrlID::Skip) == 1 || ClickBox(565, 430, 640, 455) == 1 || t.GetLapseCount() >= StartCount) { 
        Scene = SceneID::Game; InitScene(); Pause = 0; t.Start();// Forward, Skip or 3s passed
    } return;
}
void Tetris::InitScene() {
    switch (Scene) {
    case SceneID::Home: case SceneID::ModeSelect: Mode = ModeID::Initial;
    case SceneID::CharSelect: CharLv = 0; CharNo = CharID::Initial;
    case SceneID::GameStart: case SceneID::Game:
        int Rand, tmp; for (int i = 0; i < CharID::Num; i++) { ExATK[i] = false; }
        for (int i = 6; i > 0; i--) { Rand = GetRand(i); tmp = MinoBag[i]; MinoBag[i] = MinoBag[Rand]; MinoBag[Rand] = tmp; }
        for (int i = 0; i < 18; i++) {
            for (int j = 0; j < 40; j++) {
                if (!i || i == 11 || !j || j == 39) { BlockBox[i][j] = MinoID::FixedSpace; continue; } BlockBox[i][j] = MinoID::Space;
            }
        } BlockShape[7] = MinoID::Space; Index = 0; GhostMode = true; SkillMode = true; Turn = 1; Phase = 2 * CharLv + 1; 
        if (Mode > ModeID::Shuttle) Phase += 5 - CharNo; Shuttle = KeyShuttle[Phase - 1]; Score = 0; Combo = -1; 
        CT = 0; MaxCT = 10 - CharNo; MaxHP = 50000 - 100 * CharNo + 500 * CharLv; if (CharNo == 5) MaxHP = 999999; HP = MaxHP; 
        HoldCount = 0; MoveCount = 0; TotalLine = 0; TotalTetris = 0; TotalTspin = 0; Spin = 0; HoldState = 0; BackToBack = false; 
        t.Reset(); for (int i = 0; i < 2; i++) { MoveTimeCount[i] = -2; } FallTime = 100.0;
        for (int i = 0; i < Phase; i++) { FallTime -= FallAccel[i]; } 
    case SceneID::Pause: case SceneID::Result: if (Scene != SceneID::Home) break;
    default: 
        for (int i = 0; i < ModeID::Num; i++) {
            for (int j = 0; j < CharID::Num; j++) {
                cd.MaxCharLv[i][j] = 0;
            }
        }
        cd.SceneBGM[SceneID::Home] = BGMID::TETRIS; cd.SceneBGM[SceneID::ModeSelect] = BGMID::Hometown;
        cd.SceneBGM[SceneID::CharSelect] = BGMID::Detect;
        cd.BGMMode = 0; for (int i = 0; i < SoundID::Num; i++) { cd.SoundVol[i] = SoundVol::Max; } 
        ChangeSoundVol(SoundID::Total, cd.SoundVol[SoundID::Total]);
        cd.Key[CtrlID::Left]      = KEY_INPUT_LEFT; cd.Key[CtrlID::Right] = KEY_INPUT_RIGHT; 
        cd.Key[CtrlID::CCWSpin]   = KEY_INPUT_A;    cd.Key[CtrlID::CWSpin] = KEY_INPUT_D; 
        cd.Key[CtrlID::SoftDrop]  = KEY_INPUT_DOWN; cd.Key[CtrlID::HardDrop] = KEY_INPUT_S;
        cd.Key[CtrlID::ShowGhost] = KEY_INPUT_UP;   cd.Key[CtrlID::ShowSkill] = KEY_INPUT_Z; 
        cd.Key[CtrlID::Hold]      = KEY_INPUT_W;    cd.Key[CtrlID::Home] = KEY_INPUT_T; 
        cd.Key[CtrlID::Restart]   = KEY_INPUT_R;    cd.Key[CtrlID::Init] = KEY_INPUT_I; 
        cd.Key[CtrlID::Skip]      = KEY_INPUT_X;    cd.Key[CtrlID::Exit] = KEY_INPUT_Q; 
        cd.Key[CtrlID::Config]    = KEY_INPUT_C;    cd.Key[CtrlID::ChMusic] = KEY_INPUT_M; 
        cd.Key[CtrlID::ChWindow]  = KEY_INPUT_F11;  cd.Key[CtrlID::Mute] = KEY_INPUT_ESCAPE;
        cd.Key[CtrlID::Pause]     = KEY_INPUT_E; 
        cd.Key[CtrlID::Back]      = KEY_INPUT_U;    cd.Key[CtrlID::Forward] = KEY_INPUT_O; 
        cd.Key[CtrlID::Up]        = KEY_INPUT_4;    cd.Key[CtrlID::Down]    = KEY_INPUT_3; 
        for (int i = 0; i < CtrlID::Num; i++) { cd.TwinKey[i] = SINGLE; } break;
    } UpdateScene(Scene); return;
}
void Tetris::Game() {
    DrawBox(0, 25, 640, 455, BlockColor[CharNo], TRUE); DrawGraph(-70, 60, CharImage[CharNo], TRUE);
    DrawBox(110, 30, 160, 135, FrameColor, TRUE); DrawBox(160, 30, 635, 450, FrameColor, TRUE); 
    if (CharNo == CharID::Komachi && CharLv > 4) DrawStringToHandle(430, 40, "Text", StrColor, NormalFont);
    else DrawStringToHandle(430, 40, "Next", StrColor, NormalFont);
    if (CharNo == CharID::Misuzu) {
        if (HoldCount > 8 - CharLv) { DrawStringToHandle(120, 40, "Holded", StrColor, NormalFont); }
        else { DrawFormatStringToHandle(120, 40, StrColor, NormalFont, "Hold%3d", 9 - CharLv - HoldCount);
        DrawCircle(178, 48, 9, StrColor, FALSE); }
    }
    else if (HoldState > 0) DrawStringToHandle(120, 40, "NA", StrColor, NormalFont);
    else DrawStringToHandle(120, 40, "Hold", StrColor, NormalFont);
    for (int i = 1; i < 11; i++) { DrawBox(20 * i + 200, 30, 20 * i + 219, 39, BlockColor[BlockBox[i][18] % 8], TRUE); }
    for (int i = 1; i < 11; i++) {
        for (int j = 0; j < 20; j++) {
            DrawBox(20 * i + 200, 20 * j + 40, 20 * i + 219, 20 * j + 59, BlockColor[BlockBox[i][j + 19] % 8], TRUE);
            for (int k = 0; k < 4; k++) {
                if (GhostMode && BlockX[k] == i && BottomY[k] == j + 19) {
                    DrawBox(20 * i + 200, 20 * j + 40, 20 * i + 219, 20 * j + 59, BlockColor[BlockShape[0] % 8], FALSE); }
            }
        }
    }
    for (int j = 0; j < 24; j++) { if (CharNo == CharID::Komachi && j > 19 - 4 * CharLv) break;
        for (int i = 0; i < 6; i++) {
            DrawBox(15 * i + 430, 15 * j + 65, 15 * i + 444, 15 * j + 79, BlockColor[(BlockBox[11 + i][j]) % 8], TRUE);
        }
    }
    for (int i = 0; i < 6; i++) { if (CharNo == CharID::Misuzu && HoldCount > 8 - CharLv) break;
        for (int j = 0; j < 4; j++) {
            DrawBox(15 * i + 120, 15 * j + 65, 15 * i + 134, 15 * j + 79, BlockColor[(BlockBox[11 + i][j + 24]) % 8], TRUE);
        }
    }
    if (CharNo < CharID::Moscow) { DrawStringToHandle(193, 140, "CT", StrColor, NormalFont);
        for (int i = 0; i < MaxCT - 1; i++) {
            if (i < CT) { DrawBox(195, 425 - 15 * i, 209, 439 - 15 * i, c.GetColorHandle(c.Marigold), TRUE); continue; }
            DrawBox(195, 425 - 15 * i, 209, 439 - 15 * i, BlockColor[7], TRUE);
        }
    } 
    switch (CharNo) {
    case CharID::John: case CharID::Yoichi: case CharID::Moscow:
        DrawStringToHandle(0, 410, (CharName[CharNo] + " Lv." + to_string(CharLv + 1)).c_str(), BlockColor[7], NormalFont); 
        if (Mode == ModeID::Battle) DrawFormatStringToHandle(0, 430, BlockColor[7], NormalFont, "HP:%6d(%3d%%)", HP, 100 * HP / MaxHP); break;
    case CharID::Komachi: case CharID::Misuzu: case CharID::Michel:
        DrawStringToHandle(0, 410, (CharName[CharNo] + " Lv." + to_string(CharLv + 1)).c_str(), StrColor, NormalFont); 
        if (Mode == ModeID::Battle) DrawFormatStringToHandle(0, 430, StrColor, NormalFont, "HP:%6d(%3d%%)", HP, 100 * HP / MaxHP); break;
    default: exit(-1);
    } t.DrawLapseTimeToHandle(540, 40, StrColor, NormalFont);
    if (Combo < 0) DrawStringToHandle(580, 180, "\n\n\n     0", StrColor, NormalFont);
    else DrawFormatStringToHandle(580, 180, StrColor, NormalFont, "\n\n\n%6d", Combo);
    if (!t.GetLapseCount()) DrawStringToHandle(570, 180, "\n\n\n\n  0.000\n  0.000", StrColor, NormalFont);
    /*else DrawFormatStringToHandle(570, 180, StrColor, NormalFont, "\n\n\n\n%7.3f\n%7.3f",
        60.0 * (float)abs(Turn - 1) / Lapse.t, 60.0 * (float)TotalLine / Lapse.t);*/
    if (FixTimeCount > 0) DrawStringToHandle(525, 180, "\n\n\n\n\n\n\n       0.000G", StrColor, NormalFont);
    else if (ExATK[CharID::John] || HitCtrlKey(CtrlID::SoftDrop) == 1)
        DrawFormatStringToHandle(525, 180, StrColor, NormalFont, "\n\n\n\n\n\n\n%12.3lfG", 20.0 / FallTime);
    else if (HitCtrlKey(CtrlID::HardDrop) == 1) DrawFormatStringToHandle(525, 180, StrColor, NormalFont, "\n\n\n\n\n\n\n%8d.000G", 40 - BlockY[4]);
    else DrawFormatStringToHandle(525, 180, StrColor, NormalFont, "\n\n\n\n\n\n\n%12.3lfG", 1.0 / FallTime);
    if (ExATK[CharID::Komachi]) DrawBox(220, 240, 419, 439, BlockColor[CharID::Komachi], TRUE);
    if (ExATK[CharID::Misuzu] || (FixTimeCount > 0 && (HitCtrlKey(CtrlID::SoftDrop) == 1 || HitCtrlKey(CtrlID::HardDrop) == 1)))
        DrawStringToHandle(525, 180, "\n\n\n\n\n\n\n\n\n       0.000s", StrColor, NormalFont);
    else DrawFormatStringToHandle(525, 180, StrColor, NormalFont, "\n\n\n\n\n\n\n\n\n%12.3fs", (30.0 - (float)FixTimeCount) / 60.0);
    switch (Mode) {
    case ModeID::Shuttle:
        DrawStringToHandle(520, 180, "Line:", StrColor, NormalFont);
        DrawFormatStringToHandle(580, 180, StrColor, NormalFont, "%6d", TotalLine);
        DrawStringToHandle(525, 70, "Shuttle", StrColor, NormalFont);
        DrawFormatStringToHandle(520, 90, StrColor, TitleFont, "%3d", Shuttle);
        if (Shuttle >= MaxShuttle) DrawStringToHandle(80, 200, "Congratulation!!", StrColor, TitleFont);
        DrawFormatStringToHandle(420, 430, StrColor, NormalFont, "%12dm", Score);
    case ModeID::Marathon: case ModeID::Sprint:
        DrawStringToHandle(520, 180, "\nTetris:\nT-Spin:\nCombo:\nTPM:\nLPM:\nFall Speed:\n\nFix Time", StrColor, NormalFont);
        DrawFormatStringToHandle(580, 180, StrColor, NormalFont, "\n%6d\n%6d", TotalTetris, TotalTspin);
        DrawFormatStringToHandle(535, 155, StrColor, NormalFont, "Phase%3d", Phase);
        if (Turn < 0) DrawStringToHandle(200, 200, "Failed...", StrColor, TitleFont); if (Mode == ModeID::Shuttle) break;
        DrawStringToHandle(525, 70, "Line", StrColor, NormalFont); DrawStringToHandle(520, 180, "Rest:", StrColor, NormalFont);
        DrawFormatStringToHandle(520, 90, StrColor, TitleFont, "%3d", TotalLine);
        if (Mode == ModeID::Sprint) { DrawFormatStringToHandle(420, 430, StrColor, NormalFont, "%12.2fm", (float)Score / 100.0);
            if (TotalLine >= SprintLine) { DrawStringToHandle(80, 200, "Congratulation!!", StrColor, TitleFont); 
            DrawStringToHandle(580, 180, "     0", StrColor, NormalFont); break; }
            DrawFormatStringToHandle(580, 180, StrColor, NormalFont, "%6d", 40 - TotalLine); break;
        } DrawFormatStringToHandle(390, 430, StrColor, NormalFont, "%12.3fkm", (float)Score / 1000.0);
        if (TotalLine >= MarathonLine) { DrawStringToHandle(80, 200, "Congratulation!!", StrColor, TitleFont); 
        DrawStringToHandle(580, 180, "     0", StrColor, NormalFont); break; }
        DrawFormatStringToHandle(580, 180, StrColor, NormalFont, "%6d", 150 - TotalLine); break;
    case ModeID::Battle: DrawStringToHandle(525, 70, "Turn", StrColor, NormalFont);
        DrawFormatStringToHandle(520, 90, StrColor, TitleFont, "%3d", abs(Turn));
        DrawFormatStringToHandle(535, 155, StrColor, NormalFont, "Phase%3d", Phase);
        DrawStringToHandle(520, 180, "Line:\nTetris:\nT-Spin:\nCombo:\nTPM:\nLPM:\nFall Speed:\n\nFix Time", StrColor, NormalFont);
        DrawFormatStringToHandle(580, 180, StrColor, NormalFont, "%6d\n%6d\n%6d", TotalLine, TotalTetris, TotalTspin);
        DrawStringToHandle(164, 140, "HP", StrColor, NormalFont);
        DrawBox(170, 159, 184, 439 - 280 * HP / MaxHP, BlockColor[7], TRUE);
        DrawBox(170, 439 - 280 * HP / MaxHP, 184, 439, HPColor, TRUE);
        DrawFormatStringToHandle(420, 430, StrColor, NormalFont, "%12dpt", Score);
        if (!HP) DrawStringToHandle(190, 200, "Victory!!", StrColor, TitleFont);
        if (Turn < 0) DrawStringToHandle(190, 200, "Defeated...", StrColor, TitleFont); break;
    default: exit(-1);
    } return;
}
int Tetris::DrawScene() {
    DrawBox(0, 0, 640, 25, FrameColor, TRUE);
    switch (Scene) {
    case SceneID::Home: Home(); break;
    case SceneID::ModeSelect: ModeSelect(); break;
    case SceneID::CharSelect: CharSelect(); break;
    case SceneID::GameStart: GameStart(); break;
    case SceneID::Game: case SceneID::Pause: case SceneID::Result: Game(); break;
    default: Config(); break;
    } DrawBox(0, 455, 640, 480, FrameColor, TRUE); 
    if (Scene > SceneID::Home && Scene < SceneID::Game) { DrawGraph(565, 430, Icon[IconID::Skip], TRUE); }
    switch (Scene) {
    case ConfigID::Game: case ConfigID::Pause: case ConfigID::Result: 
    case SceneID::Game:  case SceneID::Pause:  case SceneID::Result:
        DrawGraph(490, 0, Icon[IconID::Ghost], TRUE); DrawGraph(515, 0, Icon[IconID::Skill], TRUE);
        DrawGraph(540, 0, Icon[IconID::Pause + Pause], TRUE);
    case ConfigID::ModeSelect: case ConfigID::CharSelect: case ConfigID::GameStart: 
    case SceneID::CharSelect: case SceneID::GameStart:
        DrawGraph(0, 455, Icon[IconID::Restart], TRUE);
    default:
        DrawGraph(0, 0, Icon[IconID::Home], TRUE); DrawGraph(25, 455, Icon[IconID::Init], TRUE);
        DrawGraph(590, 0, Icon[IconID::AnotherWindow + WindowMode], TRUE); DrawGraph(85, 0, Icon[IconID::Mute + MuteMode], TRUE);
        DrawGraph(615, 0, Icon[IconID::Exit], TRUE); DrawGraph(30, 0, Icon[IconID::Back], TRUE); 
        DrawGraph(565, 0, Icon[IconID::Config], TRUE); DrawGraph(50, 455, Icon[IconID::Music], TRUE); 
        DrawCircle(524, 468, 10, StrColor, FALSE);
        DrawStringToHandle(75, 460, BGMName[PlayingBGMNo].c_str(), StrColor, NormalFont);
        DrawStringToHandle(445, 460, "TETR4  C  2022 mini", StrColor, NormalFont); break;
    }
    switch (Scene) {
    case SceneID::Home:      DrawStringToHandle(115, 5, "Home", StrColor, NormalFont);                                          break;
    case SceneID::ModeSelect: DrawStringToHandle(115, 5, "Mode Select", StrColor, NormalFont);                                    break;
    case SceneID::CharSelect: DrawStringToHandle(115, 5, (ModeName[Mode] + " >> Charcter Select").c_str(), StrColor, NormalFont); break;
    case SceneID::GameStart:
        DrawStringToHandle(115, 5, (ModeName[Mode] + " >> " + CharName[CharNo] + " >> Game Start").c_str(), StrColor, NormalFont); break;
    case SceneID::Game: case SceneID::Pause: case SceneID::Result:
                          DrawStringToHandle(115, 5, ModeName[Mode].c_str(), StrColor, NormalFont);                           break;
    default:              DrawStringToHandle(115, 5, "Config", StrColor, NormalFont);                                         break;                                                                 break;
    } DrawCursor(); return Scene;
}
void Tetris::UpdateScene(int TargetScene) {
    switch (TargetScene) {
    case SceneID::Home: DisplayMino(2, 2, MinoID::J, 3); DisplayMino(0, 1, MinoID::T, 3); DisplayMino(6, 0, MinoID::T, 2);
        DisplayMino(1, 3, MinoID::Z, 1); DisplayMino(6, 3, MinoID::L, 1); DisplayMino(2, 4, MinoID::L, 0);
        DisplayMino(2, 0, MinoID::I, 0); DisplayMino(4, 4, MinoID::O, 0); DisplayMino(4, 2, MinoID::S, 0);
        if (Scene) PlayBGM(cd.SceneBGM[SceneID::Home]); break;
    case SceneID::ModeSelect:
        if (Scene != SceneID::ModeSelect) PlayBGM(cd.SceneBGM[SceneID::ModeSelect]); if (Mode >= 0) Mode -= ModeID::Num; break;
    case SceneID::CharSelect: if (CharNo >= 0 && Scene >= 0 && Scene != SceneID::GameStart) { CharNo -= CharID::Num; CharLv = 0; }
                          if (Scene > SceneID::CharSelect) PlayBGM(CharBGM[CharNo]);
                          else if (Scene < SceneID::CharSelect) PlayBGM(cd.SceneBGM[SceneID::CharSelect]); break;
    case SceneID::GameStart: t.Start(); if (Scene != SceneID::GameStart) PlayBGM(CharBGM[CharNo]); break;
    case SceneID::Game: if (Scene < 0) { PlayBGM(CharBGM[CharNo]); t.Resume(); break; } Shuttle++;
        if (!Mode && Shuttle >= MaxShuttle) { UpdateScene(SceneID::Result); return; } 
        FallTimeCount = 0.0; FixTimeCount = 0; MoveCount = 0;
        if (HoldCount < 2 || (!Mode && HoldState == 1) || !HoldState) { MakeBag(); NextMino(); Index++; }
        if (HP > MaxHP) HP = MaxHP; ShapeMino(5, 18, BlockX, BlockY, BlockShape[0], 0); BlockAngle = 0; BlockX[4] = 5; BlockY[4] = 18;
        for (int i = 0; i < 4; i++) { 
            if (BlockBox[BlockX[i]][BlockY[i]] > MinoID::Space) { Turn *= -1; UpdateScene(SceneID::Result); return; }
        }
        for (int i = 0; i < 4; i++) { BlockBox[BlockX[i]][BlockY[i]] = BlockShape[0]; } ShapeBottomMino();
        if (MoveTimeCount[CtrlID::Right] == -1) MoveTimeCount[CtrlID::Right] = 0; 
        else if (MoveTimeCount[CtrlID::Left] == -1) MoveTimeCount[CtrlID::Left] = 0;
        if ((!Mode && Shuttle == KeyShuttle[Phase]) || (Mode > 0 && Phase < MaxPhase && TotalLine >= 10 * (Phase + CharNo - 2 * CharLv - 5))) {
            if (!Mode && Phase >= MaxPhase) { UpdateScene(SceneID::Result); return; }
            FallTime -= FallAccel[Phase]; Phase++; PlaySoundMem(SE[2], DX_PLAYTYPE_BACK); 
            if (SkillMode) { DrawStringToHandle(150, 100, "Speed Up!", StrColor, TitleFont); ScreenFlip(); WaitTimer(400); }
        } break;
    case SceneID::Pause: Pause = 1; if (Scene < 0) PlayBGM(CharBGM[CharNo]); break;
    case SceneID::Result: if (cd.MaxCharLv[Mode][CharNo] >= 9) cd.MaxCharLv[Mode][CharNo] = 9;
    else if (Mode == ModeID::Shuttle) {
        for (int i = 0; i < 10; i++) {
            if (Phase < 2 * i + 3) { if (cd.MaxCharLv[Mode][CharNo] < i) { cd.MaxCharLv[Mode][CharNo] = i; } break; }
        }
    }
    else {
        for (int i = 0; i < 10; i++) {
            if (Phase < 2 * i + 8 - CharNo) { if (cd.MaxCharLv[Mode][CharNo] < i) { cd.MaxCharLv[Mode][CharNo] = i; } break; }
        }
    } if (Turn < 0) { PlayBGM(7); break; } HP = 0; PlayBGM(8); t.Stop(); break;
    default: if (Scene >= 0) PlayBGM(7); break;
    } Scene = TargetScene; return;
}
void Tetris::BackSystem() {
    Keyboard::GetInstance()->Update();
    switch (Scene) {
    case SceneID::Game:
        t.Update();
        if (FixTimeCount > 0) {
            if (FixTimeCount == 1) { PlaySoundMem(SE[1], DX_PLAYTYPE_BACK); }
            if (FixTimeCount >= FixTime || ExATK[CharID::Misuzu] || 
                HitCtrlKey(CtrlID::HardDrop) == 1 || HitCtrlKey(CtrlID::SoftDrop) == 1) {
                for (int i = 0; i < MinoID::BlockNum; i++) { BlockBox[BlockX[i]][BlockY[i]] = BlockShape[0] + MinoID::Num; } ClearLine(); 
                if ((Mode == ModeID::Sprint && TotalLine >= SprintLine) || (Mode == ModeID::Marathon && TotalLine >= MarathonLine)) { 
                    UpdateScene(SceneID::Result); return; }
                UpdateScene(SceneID::Game); return;
            } FixTimeCount++;
        }
        else {
            if (ExATK[CharID::John] || HitCtrlKey(CtrlID::SoftDrop) > 0) { FallTimeCount += 19.0; }
            while (FallTimeCount > FallTime) { DropMino(); if (ExATK[CharID::John] || HitCtrlKey(CtrlID::SoftDrop) > 0) { 
                Score += 3; HP -= 3; } 
            }
            FallTimeCount += 1.0; if (HitCtrlKey(CtrlID::HardDrop) == 1) { HardDropMino(); }
        }
        if (HitCtrlKey(CtrlID::Hold) == 1) { HoldMino(); return; }
        if (HitCtrlKey(CtrlID::CCWSpin) == 1) AntiClockSpinMino(); else if (HitCtrlKey(CtrlID::CWSpin) == 1) ClockSpinMino();
        if (HitCtrlKey(CtrlID::Left) == 1) { if (MoveTimeCount[CtrlID::Right] > -2) { 
            MoveTimeCount[CtrlID::Right] = -2; } MoveLeftMino(); 
        }
        else if (HitCtrlKey(CtrlID::Right) == 1) { 
            if (MoveTimeCount[CtrlID::Left] > -2) { MoveTimeCount[CtrlID::Left] = -2; } MoveRightMino(); 
        }
        if (HitCtrlKey(CtrlID::Left) == 18) MoveTimeCount[CtrlID::Left] = 2; 
        else if (HitCtrlKey(CtrlID::Right) == 18) MoveTimeCount[CtrlID::Right] = 2;
        if (MoveTimeCount[CtrlID::Left] >= 0) { 
            MoveTimeCount[CtrlID::Left]++; if (MoveTimeCount[CtrlID::Left] > 2) { MoveTimeCount[CtrlID::Left] = MoveLeftMino(); } 
        }
        else if (MoveTimeCount[CtrlID::Right] >= 0) { 
            MoveTimeCount[CtrlID::Right]++; if (MoveTimeCount[CtrlID::Right] > 2) { MoveTimeCount[CtrlID::Right] = MoveRightMino(); } 
        }
        if ((Mode == ModeID::Battle && HP <= 0)) { UpdateScene(SceneID::Result); return; }
    case SceneID::Pause:
        if (ClickBox(540, 0, 565, 25) == 1 || HitCtrlKey(CtrlID::Pause) == 1) {
            if (!Pause) { UpdateScene(SceneID::Pause); t.Stop(); return; } Pause = 0; t.Resume(); Scene = SceneID::Game; return;
        }
    case SceneID::Result:
        if (ClickBox(30, 0, 55, 25) == 1 || HitCtrlKey(CtrlID::Back) == 1) { UpdateScene(SceneID::GameStart); return; }
    case ConfigID::ModeSelect: case ConfigID::CharSelect: case ConfigID::GameStart: case ConfigID::Game: 
    case ConfigID::Pause: case ConfigID::Result: case SceneID::CharSelect: case SceneID::GameStart:
        if (ClickBox(0, 455, 25, 480) == 1 || HitCtrlKey(CtrlID::Restart) == 1) { UpdateScene(SceneID::ModeSelect); return; }
    default:
        if (ClickBox(490, 0, 515, 25) == 1 || HitCtrlKey(CtrlID::ShowGhost) == 1) GhostMode = !GhostMode;
        else if (ClickBox(515, 0, 540, 25) == 1 || HitCtrlKey(CtrlID::ShowSkill) == 1) SkillMode = !SkillMode;
        else if (ClickBox(25, 455, 50, 480) == 1 || HitCtrlKey(CtrlID::Init) == 1) InitScene();
        else if (ClickBox(0, 0, 28, 25) == 1 || HitCtrlKey(CtrlID::Home) == 1) UpdateScene(SceneID::Home);
        else if (ClickBox(50, 455, 75, 480) == 1 || HitCtrlKey(CtrlID::ChMusic) == 1) PlayBGM((PlayingBGMNo + 1) % BGMID::Num);
        else if (ClickBox(565, 0, 590, 25) == 1 || HitCtrlKey(CtrlID::Config) == 1) {
            if (Scene == SceneID::Game) { Pause = 1; Scene = SceneID::Pause; t.Stop(); }
            if (Scene >= 0) { UpdateScene(Scene - SceneID::Num); } else { UpdateScene(Scene + SceneID::Num); }
        }
        else if (ClickBox(590, 0, 615, 25) == 1 || HitCtrlKey(CtrlID::ChWindow) == 1) ChangeWindow((WindowMode + 1) % 2);
        else if (ClickBox(85, 0, 110, 25) == 1 || HitCtrlKey(CtrlID::Mute) == 1) ChangeSound((MuteMode + 1) % 2);
        else if (HitCtrlKey(CtrlID::Down) > 10) ChangeSoundVol(SoundID::Total, cd.SoundVol[SoundID::Total] - 1);
        else if (HitCtrlKey(CtrlID::Up)   > 10) ChangeSoundVol(SoundID::Total, cd.SoundVol[SoundID::Total] + 1);
        else if (HitCtrlKey(CtrlID::Down) == 1) ChangeSoundVol(SoundID::Total, cd.SoundVol[SoundID::Total] - 4);
        else if (HitCtrlKey(CtrlID::Up)   == 1) ChangeSoundVol(SoundID::Total, cd.SoundVol[SoundID::Total] + 4);
        else if (MuteMode && !CheckSoundMem(BGM[PlayingBGMNo])) {
            switch (cd.BGMMode) {
            case BGMModeID::Ascending: PlayBGM((PlayingBGMNo + 1) % BGMID::Num); return;
            case BGMModeID::Descending: PlayBGM((PlayingBGMNo + BGMID::Num - 1) % BGMID::Num); return;
            case BGMModeID::Random: PlayBGM(GetRand(BGMID::Num - 1));
            default: return;
            }
        }
    } return;
}
int Tetris::DrawCursor() {
    Mouse::GetInstance()->Update(); CursorX = Mouse::GetInstance()->getX(); CursorY = Mouse::GetInstance()->getY();
    return DrawGraph(CursorX - 15, CursorY - 15, Icon[IconID::Cursor], TRUE);
}
int Tetris::ClearLine() {
    int Line = 0; int Tspin = 0; int PC = 0; int BTB = 0; bool Bonus = false; int Heal = 0; bool Skill = false;
    int LineMinoCount = 0; int SuccessLine[4] = {}; int SpaceCount = 0; int Damage = 0; int TspinObj = 0;
    int s = BlockAngle % 4; int t = (BlockAngle + 1) % 4; int u = (BlockAngle + 2) % 4; int v = (BlockAngle + 3) % 4;
    if (Spin > SpinID::Non && BlockShape[0] == MinoID::T) {
        for (int i = 0; i < 4; i++) {
            CheckX[i] = BlockX[4] + CompX[(BlockAngle + i) % 4] + CompY[(BlockAngle + i) % 4];
            CheckY[i] = BlockY[4] + CompX[(BlockAngle + i + 1) % 4] + CompY[(BlockAngle + i + 1) % 4];
            if (BlockBox[CheckX[i]][CheckY[i]] > MinoID::Space) { TspinObj++; if (i < 2) { TspinObj++; } continue; } TspinObj--;
        }
    } if (TspinObj == 3 && Spin == SpinID::Special) TspinObj = 5;
    for (int j = 38; j > 1; j--) {
        LineMinoCount = 0;
        for (int i = 1; i < 11; i++) {
            if (BlockBox[i][j] > MinoID::Space) LineMinoCount++;
        }
        if (LineMinoCount == 10) {
            SuccessLine[Line] = j - Line - 19;
            for (int i = 1; i < 11; i++) {
                for (int k = j; k > 1; k--) {
                    BlockBox[i][k] = BlockBox[i][k - 1];
                }
            } Line++; Skill = true; j++;
        }
    }
    for (int i = 1; i < 11; i++) { if (BlockBox[i][38] == MinoID::Space) SpaceCount++; } TotalLine += Line;
    if (SpaceCount == 10) { PC = 1; Skill = true; PlaySoundMem(SE[3], DX_PLAYTYPE_BACK);
    }
    for (int j = 0; j < Line; j++) {
        for (int i = 1; i < 11; i++) {
            DrawBox(20 * i + 200, 20 * SuccessLine[j] + 40, 20 * i + 219, 20 * SuccessLine[j] + 59, BlockColor[MinoID::S], TRUE);
        }
    } 
    if (TspinObj == 3) { Tspin = 1; Skill = true; Damage += 100; }
    else if (TspinObj > 3) {
        if (BackToBack) { BTB = 1; } Tspin = 2; Skill = true; TotalTspin++; PlaySoundMem(SE[3], DX_PLAYTYPE_BACK);
        switch (Line) {
        case 0: Damage += 400;  if (BackToBack) Damage += 200; break;
        case 1: Damage += 800;  if (BackToBack) Damage += 400; break;
        case 2: Damage += 1200; if (BackToBack) Damage += 600; break;
        case 3: Damage += 1600; if (BackToBack) Damage += 800; break;
        default: exit(-1);
        }
    }
    switch (Line) {
    case 0: if (TspinObj > 3) BackToBack = true; Combo = -1; break;
    case 1: Combo++; Damage = 100 + 50 * Combo; if (BackToBack) Damage += 50 + 25 * Combo; if (SpaceCount == 10) Score += 800;
        if (TspinObj > 3) BackToBack = true; else BackToBack = false; break;
    case 2: Combo++; Damage = 300 + 50 * Combo; if (BackToBack) Damage += 150 + 25 * Combo; if (SpaceCount == 10) Score += 1000;
        if (TspinObj > 3) BackToBack = true; else BackToBack = false; break;
    case 3: Combo++; Damage = 500 + 50 * Combo; if (BackToBack) Damage += 250 + 25 * Combo; if (SpaceCount == 10) Score += 1800;
        if (TspinObj > 3) BackToBack = true; else BackToBack = false; break;
    case 4: Combo++; Damage = 800 + 50 * Combo; if (BackToBack) Damage += 400 + 25 * Combo; if (SpaceCount == 10) Score += 2000;
        if (BackToBack) { BTB = 1; Skill = true; }
        BackToBack = true; TotalTetris++; PlaySoundMem(SE[3], DX_PLAYTYPE_BACK); break;
    default: exit(-1);
    } 
    Score += 3 * Damage; HP -= 3 * Damage; if (Line) PlaySoundMem(SE[0], DX_PLAYTYPE_BACK); 
    if (Skill) { if (!CT) { Bonus = true; HP -= 1000; } if (CT > 0) { CT--; } } CT = (CT + 1) % MaxCT; Turn++;
    for (int i = 0; i < CharID::Num; i++) { ExATK[i] = false; } if (!CT) ExATK[CharNo] = true; 
    if (ExATK[CharID::Yoichi]) ExATK[GetRand(3)] = true;
    if (ExATK[CharID::Michel]) { AddMino(); 
    if (Mode == ModeID::Battle) { Heal = 10 * (GetRand(10) + CharLv + 1); HP += Heal; Skill = true; } }
    if (HoldState > 0) { HoldState--; } Spin = SpinID::Non;
    if (Combo > 0) { PlaySoundMem(ComboSE[(Combo - 1) % SEID::ComboNum], DX_PLAYTYPE_BACK); }
    if (Mode == ModeID::Battle && CharNo == CharID::Moscow) { Heal = MaxHP - HP; HP = MaxHP; }
    if (SkillMode && Skill) {
        if (Mode == ModeID::Battle) {
            if (Damage > 0) { DrawFormatStringToHandle(150, 240, StrColor, NormalFont, "-%d", Damage); }
            if (Heal > 0) { DrawFormatStringToHandle(150, 180, BlockColor[2], NormalFont, "+%d", Heal); }
            if (Bonus) { DrawStringToHandle(150, 270, "-1000", BlockColor[1], NormalFont); }
        }
        if (Line > 0) { DrawStringToHandle(160, 180, SkillName[Line - 1].c_str(), StrColor, TitleFont);
            if (Combo > 0) { DrawFormatStringToHandle(270, 240, BlockColor[Combo % 7], TitleFont, "%4d Combo!", Combo); }
        }
        if (PC == 1) { DrawStringToHandle(120, 60, "Perfect Clear!!", BlockColor[0], TitleFont); }
        if (Tspin == 2) { DrawStringToHandle(30, 260, "T-Spin!!", BlockColor[3], TitleFont); }
        else if (Tspin == 1) { DrawStringToHandle(0, 290, "T-Spin Mini!", BlockColor[2], TitleFont); }
        if (BTB == 1) { DrawStringToHandle(200, 120, "Back To Back!!", BlockColor[6], TitleFont); }
        ScreenFlip(); WaitTimer((Line + PC + Tspin + BTB) * 20 + Combo * 5 + 300);
    } 
    return Line;
}
void Tetris::AddMino() {
    for (int i = 1; i < 11; i++) {
        for (int j = 1; j < BlockBoxData::BottomNo - 1; j++) {
            BlockBox[i][j] = BlockBox[i][j + 1];
        }
    } int SpaceX = GetRand(9) + 1;
    for (int i = 1; i < 11; i++) { 
        if (i != SpaceX) { 
            BlockBox[i][BlockBoxData::BottomNo - 1] = MinoID::FixedT; continue; 
        } BlockBox[i][BlockBoxData::BottomNo - 1] = MinoID::Space; 
    }
    return;
}
int Tetris::HoldMino() {
    if (HoldState > 0 || (CharNo == 2 && HoldCount > 8 - CharLv)) return -1; if (DeleteMino()) return -1;
    ShapeNextMino(MinoID::TypeNum, MinoID::Space);
    if (!HoldCount) BlockShape[MinoID::TypeNum] = BlockShape[0]; 
    else { int tmp = BlockShape[MinoID::TypeNum]; BlockShape[MinoID::TypeNum] = BlockShape[0]; BlockShape[0] = tmp; }
    ShapeNextMino(7, BlockShape[7]); HoldCount++; if (!Mode) HoldState = 2; else HoldState = 1; 
    UpdateScene(SceneID::Game); return 0;
}
int Tetris::DeleteMino() {
    ShapeMino(BlockX[MinoID::Center], BlockY[MinoID::Center], BlockX, BlockY, BlockShape[0], BlockAngle);
    // Once tetrimino land, the time of its move is up to 15
    if (FixTimeCount > 0) { MoveCount++; if (MoveCount >= 15) { FixTimeCount = FixTime; return -1; } FixTimeCount = 0; }
    for (int i = 0; i < MinoID::BlockNum; i++) { BlockBox[BlockX[i]][BlockY[i]] = MinoID::Space; } return 0;
}
int Tetris::MakeBag() {
    int Rand, tmp;
    if (!(Index % MinoID::BagNum)) {
        for (int i = MinoID::BagNum - 1; i > MinoID::TypeNum; i--) { 
            Rand = GetRand(i - MinoID::TypeNum) + MinoID::TypeNum; tmp = MinoBag[i]; MinoBag[i] = MinoBag[Rand]; MinoBag[Rand] = tmp; 
        } return 0;
    }
    if (Index % MinoID::BagNum == MinoID::TypeNum) {
        for (int i = MinoID::TypeNum - 1; i > 0; i--) { 
            Rand = GetRand(i); tmp = MinoBag[i]; MinoBag[i] = MinoBag[Rand]; MinoBag[Rand] = tmp; 
        } return 0;
    } return -1;
}
void Tetris::NextMino() {
    BlockShape[0] = MinoBag[Index % MinoID::BagNum];
    for (int i = 1; i < MinoID::TypeNum; i++) {
        ShapeNextMino(i, MinoID::Space); BlockShape[i] = MinoBag[(Index + i) % MinoID::BagNum]; ShapeNextMino(i, BlockShape[i]);
    } return;
}
void Tetris::ShapeNextMino(int MinoNo, int Kind) {
    BlockBox[13][MinoNo * 4 - 2] = Kind;
    switch (BlockShape[MinoNo]) {
    case MinoID::O: BlockBox[13][MinoNo * 4 - 3] = Kind; BlockBox[14][MinoNo * 4 - 3] = Kind; BlockBox[14][MinoNo * 4 - 2] = Kind; return;
    case MinoID::I: BlockBox[12][MinoNo * 4 - 2] = Kind; BlockBox[14][MinoNo * 4 - 2] = Kind; BlockBox[15][MinoNo * 4 - 2] = Kind; return;
    case MinoID::Z: BlockBox[12][MinoNo * 4 - 3] = Kind; BlockBox[13][MinoNo * 4 - 3] = Kind; BlockBox[14][MinoNo * 4 - 2] = Kind; return;
    case MinoID::T: BlockBox[13][MinoNo * 4 - 3] = Kind; BlockBox[12][MinoNo * 4 - 2] = Kind; BlockBox[14][MinoNo * 4 - 2] = Kind; return;
    case MinoID::L: BlockBox[14][MinoNo * 4 - 3] = Kind; BlockBox[12][MinoNo * 4 - 2] = Kind; BlockBox[14][MinoNo * 4 - 2] = Kind; return;
    case MinoID::S: BlockBox[13][MinoNo * 4 - 3] = Kind; BlockBox[14][MinoNo * 4 - 3] = Kind; BlockBox[12][MinoNo * 4 - 2] = Kind; return;
    case MinoID::J: BlockBox[12][MinoNo * 4 - 3] = Kind; BlockBox[12][MinoNo * 4 - 2] = Kind; BlockBox[14][MinoNo * 4 - 2] = Kind; return;
    case MinoID::Space: BlockBox[12][MinoNo * 4 - 2] = Kind; BlockBox[14][MinoNo * 4 - 2] = Kind; BlockBox[15][MinoNo * 4 - 2] = Kind;
                        BlockBox[12][MinoNo * 4 - 3] = Kind; BlockBox[13][MinoNo * 4 - 3] = Kind; BlockBox[14][MinoNo * 4 - 3] = Kind;
                        BlockBox[15][MinoNo * 4 - 3] = Kind; return;
    default: return;
    } return;
}
int Tetris::ShapeMino(int x, int y, int ShapeX[], int ShapeY[], int Kind, int Angle) {
    int s = Angle % 4; int t = (Angle + 1) % 4; int u = (Angle + 2) % 4; int v = (Angle + 3) % 4;
    if (Kind % MinoID::Num != MinoID::I) { ShapeX[0] = x; ShapeY[0] = y; }
    switch (Kind % MinoID::Num) {
    case MinoID::O: ShapeX[1] = x + 1; ShapeY[1] = y; ShapeX[2] = x + 1; ShapeY[2] = y - 1;
                    ShapeX[3] = x; ShapeY[3] = y - 1; break;
    case MinoID::I: ShapeX[0] = x + Ic[s]; ShapeY[0] = y + Ic[t]; ShapeX[1] = x + Ic[v]; ShapeY[1] = y + Ic[s];
                    ShapeX[2] = x + Im[s]; ShapeY[2] = y + Im[t]; ShapeX[3] = x + Ie[s]; ShapeY[3] = y + Ie[t]; break;
    case MinoID::Z: ShapeX[1] = x + CompX[s]; ShapeY[1] = y + CompY[s]; ShapeX[2] = x + CompX[t]; ShapeY[2] = y + CompY[t];
                    ShapeX[3] = x + CompX[t] + CompY[t]; ShapeY[3] = y + CompX[u] + CompY[u]; break;
    case MinoID::T: ShapeX[1] = x + CompX[s]; ShapeY[1] = y + CompY[s]; ShapeX[2] = x + CompX[t]; ShapeY[2] = y + CompY[t];
                    ShapeX[3] = x + CompX[u]; ShapeY[3] = y + CompY[u]; break;
    case MinoID::L: ShapeX[1] = x + CompX[s]; ShapeY[1] = y + CompY[s]; ShapeX[2] = x + CompX[u]; ShapeY[2] = y + CompY[u];
                    ShapeX[3] = x + CompX[s] + CompY[s]; ShapeY[3] = y + CompX[t] + CompY[t]; break;
    case MinoID::S: ShapeX[1] = x + CompX[t]; ShapeY[1] = y + CompY[t]; ShapeX[2] = x + CompX[u]; ShapeY[2] = y + CompY[u];
                    ShapeX[3] = x + CompX[s] + CompY[s]; ShapeY[3] = y + CompX[t] + CompY[t]; break;
    case MinoID::J: ShapeX[1] = x + CompX[s]; ShapeY[1] = y + CompY[s]; ShapeX[2] = x + CompX[u]; ShapeY[2] = y + CompY[u];
                    ShapeX[3] = x + CompX[t] + CompY[t]; ShapeY[3] = y + CompX[u] + CompY[u]; break;
    default: return -1;
    } return 0;
}
int Tetris::DisplayMino(int x, int y, int MinoShape, int MinoAngle) {
    if (ShapeMino(x, y, BlockX, BlockY, MinoShape, MinoAngle)) return -1;
    for (int i = 0; i < MinoID::BlockNum; i++) { BlockBox[BlockX[i]][BlockY[i]] = MinoShape; } return 0;
}
int Tetris::ShapeBottomMino() {
    for (int j = 0; j < 41 - BlockY[MinoID::Center]; j++) {
        for (int i = 0; i < MinoID::BlockNum; i++) {
            if (BlockBox[BlockX[i]][BlockY[i] + j] > MinoID::Space) {
                if (j == 1 && !FixTimeCount) { FixTimeCount++; } j--; BottomY[MinoID::Center] = BlockY[MinoID::Center] + j;
                for (int k = 0; k < MinoID::BlockNum; k++) { BottomY[k] = BlockY[k] + j; } return 0;
            }
        }
    } return -1;
}
int Tetris::MakeMino(int x, int y, int Angle) {
    ShapeMino(x, y, CheckX, CheckY, BlockShape[0], Angle);
    for (int i = 0; i < MinoID::BlockNum; i++) { if (BlockBox[CheckX[i]][CheckY[i]] > MinoID::Space) { return -1; } } 
    if (DeleteMino()) return -1; BlockAngle = Angle; BlockX[MinoID::Center] = x; BlockY[MinoID::Center] = y;
    for (int i = 0; i < MinoID::BlockNum; i++) { 
        BlockX[i] = CheckX[i]; BlockY[i] = CheckY[i]; BlockBox[BlockX[i]][BlockY[i]] = BlockShape[0]; 
    } ShapeBottomMino(); return 0;
}
int Tetris::ClockSpinMino() {
    if (BlockShape[0] == MinoID::O) return 0; int CheckAngle = (BlockAngle + 3) % 4;
    if (MakeMino(BlockX[MinoID::Center], BlockY[MinoID::Center], CheckAngle)) {
        // Move to SRS
        if (BlockShape[0] == MinoID::I) {
            switch (BlockAngle) {
            case 0: 
                if (MakeMino(BlockX[MinoID::Center] - 2, BlockY[MinoID::Center], CheckAngle)) {
                    if (MakeMino(BlockX[MinoID::Center] + 1, BlockY[MinoID::Center], CheckAngle)) {
                        if (MakeMino(BlockX[MinoID::Center] - 2, BlockY[MinoID::Center] + 1, CheckAngle)) {
                            if (MakeMino(BlockX[MinoID::Center] + 1, BlockY[MinoID::Center] - 2, CheckAngle)) { return -1; } 
                            Spin = SpinID::Special; return 0;
                        }
                    }
                } break;
            case 1: 
                if (MakeMino(BlockX[MinoID::Center] - 2, BlockY[MinoID::Center], CheckAngle)) {
                    if (MakeMino(BlockX[MinoID::Center] + 1, BlockY[MinoID::Center], CheckAngle)) {
                        if (MakeMino(BlockX[MinoID::Center] + 1, BlockY[MinoID::Center] + 2, CheckAngle)) {
                            if (MakeMino(BlockX[MinoID::Center] - 2, BlockY[MinoID::Center] - 1, CheckAngle)) { return -1; }
                            Spin = SpinID::Special; return 0;
                        }
                    }
                } break;
            case 2: 
                if (MakeMino(BlockX[MinoID::Center] + 2, BlockY[MinoID::Center], CheckAngle)) {
                    if (MakeMino(BlockX[MinoID::Center] - 1, BlockY[MinoID::Center], CheckAngle)) {
                        if (MakeMino(BlockX[MinoID::Center] + 2, BlockY[MinoID::Center] - 1, CheckAngle)) {
                            if (MakeMino(BlockX[MinoID::Center] - 1, BlockY[MinoID::Center] + 2, CheckAngle)) { return -1; } 
                            Spin = SpinID::Special; return 0;
                        }
                    }
                } break;
            case 3: 
                if (MakeMino(BlockX[MinoID::Center] - 1, BlockY[MinoID::Center], CheckAngle)) {
                    if (MakeMino(BlockX[MinoID::Center] + 2, BlockY[MinoID::Center], CheckAngle)) {
                        if (MakeMino(BlockX[MinoID::Center] - 1, BlockY[MinoID::Center] - 2, CheckAngle)) {
                            if (MakeMino(BlockX[MinoID::Center] + 2, BlockY[MinoID::Center] + 1, CheckAngle)) { return -1; } 
                            Spin = SpinID::Special; return 0;
                        }
                    }
                } break;
            default: exit(-1);
            }
        }
        else {
            switch (BlockAngle) {
            case 0: 
                if (MakeMino(BlockX[MinoID::Center] - 1, BlockY[MinoID::Center], CheckAngle)) {
                    if (MakeMino(BlockX[MinoID::Center] - 1, BlockY[MinoID::Center] - 1, CheckAngle)) {
                        if (MakeMino(BlockX[MinoID::Center], BlockY[MinoID::Center] + 2, CheckAngle)) {
                            if (MakeMino(BlockX[MinoID::Center] - 1, BlockY[MinoID::Center] + 2, CheckAngle)) { return -1; } 
                            Spin = SpinID::Special; return 0;
                        }
                    }
                } break;
            case 1: 
                if (MakeMino(BlockX[MinoID::Center] - 2, BlockY[MinoID::Center], CheckAngle)) {
                    if (MakeMino(BlockX[MinoID::Center] - 2, BlockY[MinoID::Center] + 1, CheckAngle)) {
                        if (MakeMino(BlockX[MinoID::Center], BlockY[MinoID::Center] - 2, CheckAngle)) {
                            if (MakeMino(BlockX[MinoID::Center] - 1, BlockY[MinoID::Center] - 2, CheckAngle)) { return -1; } 
                            Spin = SpinID::Special; return 0;
                        }
                    }
                } break;
            case 2: 
                if (MakeMino(BlockX[MinoID::Center] + 1, BlockY[MinoID::Center], CheckAngle)) {
                    if (MakeMino(BlockX[MinoID::Center] + 1, BlockY[MinoID::Center] - 1, CheckAngle)) {
                        if (MakeMino(BlockX[MinoID::Center], BlockY[MinoID::Center] + 2, CheckAngle)) {
                            if (MakeMino(BlockX[MinoID::Center] + 1, BlockY[MinoID::Center] + 2, CheckAngle)) { return -1; } 
                            Spin = SpinID::Special; return 0;
                        }
                    }
                } break;
            case 3: 
                if (MakeMino(BlockX[MinoID::Center] + 1, BlockY[MinoID::Center], CheckAngle)) {
                    if (MakeMino(BlockX[MinoID::Center] + 1, BlockY[MinoID::Center] + 1, CheckAngle)) {
                        if (MakeMino(BlockX[MinoID::Center], BlockY[MinoID::Center] - 2, CheckAngle)) {
                            if (MakeMino(BlockX[MinoID::Center] + 1, BlockY[MinoID::Center] - 2, CheckAngle)) { return -1; } 
                            Spin = SpinID::Special; return 0;
                        }
                    }
                } break;
            default: exit(-1);
            }
        }
    } Spin = SpinID::Normal; return 0;
}
int Tetris::AntiClockSpinMino() {
    if (BlockShape[0] == MinoID::O) return 0; int CheckAngle = (BlockAngle + 1) % 4;
    if (MakeMino(BlockX[MinoID::Center], BlockY[MinoID::Center], CheckAngle)) {
        // Move to SRS
        if (BlockShape[0] == MinoID::I) {
            switch (BlockAngle) {
            case 0: 
                if (MakeMino(BlockX[MinoID::Center] - 1, BlockY[MinoID::Center], CheckAngle)) {
                    if (MakeMino(BlockX[MinoID::Center] + 2, BlockY[MinoID::Center], CheckAngle)) {
                        if (MakeMino(BlockX[MinoID::Center] - 1, BlockY[MinoID::Center] - 2, CheckAngle)) {
                            if (MakeMino(BlockX[MinoID::Center] + 2, BlockY[MinoID::Center] + 1, CheckAngle)) { return -1; } 
                            Spin = SpinID::Special; return 0;
                        }
                    }
                } break;
            case 1: 
                if (MakeMino(BlockX[MinoID::Center] + 1, BlockY[MinoID::Center], CheckAngle)) {
                    if (MakeMino(BlockX[MinoID::Center] - 2, BlockY[MinoID::Center], CheckAngle)) {
                        if (MakeMino(BlockX[MinoID::Center] - 2, BlockY[MinoID::Center] + 1, CheckAngle)) {
                            if (MakeMino(BlockX[MinoID::Center] + 1, BlockY[MinoID::Center] - 2, CheckAngle)) { return -1; } 
                            Spin = SpinID::Special; return 0;
                        }
                    }
                } break;
            case 2: 
                if (MakeMino(BlockX[MinoID::Center] + 1, BlockY[MinoID::Center], CheckAngle)) {
                    if (MakeMino(BlockX[MinoID::Center] - 2, BlockY[MinoID::Center], CheckAngle)) {
                        if (MakeMino(BlockX[MinoID::Center] + 1, BlockY[MinoID::Center] + 2, CheckAngle)) {
                            if (MakeMino(BlockX[MinoID::Center] - 2, BlockY[MinoID::Center] - 1, CheckAngle)) { return -1; } 
                            Spin = SpinID::Special; return 0;
                        }
                    }
                } break;
            case 3: 
                if (MakeMino(BlockX[MinoID::Center] + 2, BlockY[MinoID::Center], CheckAngle)) {
                    if (MakeMino(BlockX[MinoID::Center] - 1, BlockY[MinoID::Center], CheckAngle)) {
                        if (MakeMino(BlockX[MinoID::Center] + 2, BlockY[MinoID::Center] - 1, CheckAngle)) {
                            if (MakeMino(BlockX[MinoID::Center] - 1, BlockY[MinoID::Center] + 2, CheckAngle)) { return -1; } 
                            Spin = SpinID::Special; return 0;
                        }
                    }
                } break;
            default: exit(-1);
            }
        }
        else {
            switch (BlockAngle) {
            case 0: 
                if (MakeMino(BlockX[MinoID::Center] + 1, BlockY[MinoID::Center], CheckAngle)) {
                    if (MakeMino(BlockX[MinoID::Center] + 1, BlockY[MinoID::Center] - 1, CheckAngle)) {
                        if (MakeMino(BlockX[MinoID::Center], BlockY[MinoID::Center] + 2, CheckAngle)) {
                            if (MakeMino(BlockX[MinoID::Center] + 1, BlockY[MinoID::Center] + 2, CheckAngle)) { return -1; } 
                            Spin = SpinID::Special; return 0;
                        }
                    }
                } break;
            case 1: 
                if (MakeMino(BlockX[MinoID::Center] - 1, BlockY[MinoID::Center], CheckAngle)) {
                    if (MakeMino(BlockX[MinoID::Center] - 1, BlockY[MinoID::Center] + 1, CheckAngle)) {
                        if (MakeMino(BlockX[MinoID::Center], BlockY[MinoID::Center] - 2, CheckAngle)) {
                            if (MakeMino(BlockX[MinoID::Center] - 1, BlockY[MinoID::Center] - 2, CheckAngle)) { return -1; } 
                            Spin = SpinID::Special; return 0;
                        }
                    }
                } break;
            case 2: 
                if (MakeMino(BlockX[MinoID::Center] - 1, BlockY[MinoID::Center], CheckAngle)) {
                    if (MakeMino(BlockX[MinoID::Center] - 1, BlockY[MinoID::Center] - 1, CheckAngle)) {
                        if (MakeMino(BlockX[MinoID::Center], BlockY[MinoID::Center] + 2, CheckAngle)) {
                            if (MakeMino(BlockX[MinoID::Center] - 1, BlockY[MinoID::Center] + 2, CheckAngle)) { return -1; } 
                            Spin = SpinID::Special; return 0;
                        }
                    }
                } break;
            case 3: 
                if (MakeMino(BlockX[MinoID::Center] + 1, BlockY[MinoID::Center], CheckAngle)) {
                    if (MakeMino(BlockX[MinoID::Center] + 1, BlockY[MinoID::Center] + 1, CheckAngle)) {
                        if (MakeMino(BlockX[MinoID::Center], BlockY[MinoID::Center] - 2, CheckAngle)) {
                            if (MakeMino(BlockX[MinoID::Center] + 1, BlockY[MinoID::Center] - 2, CheckAngle)) { return -1; } 
                            Spin = SpinID::Special; return 0;
                        }
                    }
                } break;
            default: exit(-1);
            }
        }
    } Spin = SpinID::Normal; return 0;
}
int Tetris::MoveLeftMino() { 
    for (int i = 0; i < MinoID::BlockNum; i++) { if (BlockBox[BlockX[i] - 1][BlockY[i]] > MinoID::Space) return -1; } 
    if (DeleteMino()) return -1; BlockX[MinoID::Center]--;
    for (int i = 0; i < MinoID::BlockNum; i++) { BlockX[i]--; BlockBox[BlockX[i]][BlockY[i]] = BlockShape[0]; } 
    ShapeBottomMino(); Spin = 0; return 0;
}
int Tetris::MoveRightMino() { 
    for (int i = 0; i < MinoID::BlockNum; i++) { if (BlockBox[BlockX[i] + 1][BlockY[i]] > MinoID::Space) return -1; } 
    if (DeleteMino()) return -1; BlockX[MinoID::Center]++;
    for (int i = 0; i < MinoID::BlockNum; i++) { BlockX[i]++; BlockBox[BlockX[i]][BlockY[i]] = BlockShape[0]; } 
    ShapeBottomMino(); Spin = 0; return 0;
}
int Tetris::DropMino() { FallTimeCount -= FallTime; 
    for (int i = 0; i < MinoID::BlockNum; i++) { if (BlockBox[BlockX[i]][BlockY[i] + 1] > MinoID::Space) { FixTimeCount++; return -1; } }
    if (DeleteMino()) return -1; BlockY[MinoID::Center]++;
    for (int i = 0; i < MinoID::BlockNum; i++) { BlockY[i]++; BlockBox[BlockX[i]][BlockY[i]] = BlockShape[0]; }
    for (int i = 0; i < MinoID::BlockNum; i++) { if (BlockBox[BlockX[i]][BlockY[i] + 1] > MinoID::Space) { FixTimeCount++; break; } } 
    Spin = 0; return 0;
}
int Tetris::HardDropMino() { if (DeleteMino()) return -1; 
    Score += 6 * (BottomY[MinoID::Center] - BlockY[MinoID::Center]); HP -= 6 * (BottomY[MinoID::Center] - BlockY[MinoID::Center]); 
    BlockY[MinoID::Center] = BottomY[MinoID::Center];
    for (int i = 0; i < MinoID::BlockNum; i++) { BlockBox[BlockX[i]][BottomY[i]] = BlockShape[0]; BlockY[i] = BottomY[i]; }
    FixTimeCount++; Spin = 0; return 0;
}
Tetris::~Tetris() {
    FILE* ocdf = fopen(DataFileName, "wb+"); fwrite(&cd, sizeof(cd), 1, ocdf); fclose(ocdf); 
    PlaySound(NULL, 0, 0); InitSoundMem(); DxLib_End();
}

