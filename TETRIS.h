#pragma once
// Control No
#define KeyNum              67
#define MaxKeyNum          212
// TwinKey No
#define SINGLE               0
#define SHIFT                1  
#define CTRL                 2  
#define ALT                  3
#include "Color.h"
#include "Timer.h"
#include <iostream>
#include <string>
#include <complex>
using namespace std;
struct SceneID {
    static constexpr int Home       = 0;
    static constexpr int ModeSelect = 1;
    static constexpr int CharSelect = 2;
    static constexpr int GameStart  = 3;
    static constexpr int Game       = 4;
    static constexpr int Pause      = 5;
    static constexpr int Result     = 6;
    static constexpr int Num        = 7;
};
struct ConfigID {
    static constexpr int Home       = -7;
    static constexpr int ModeSelect = -6;
    static constexpr int CharSelect = -5;
    static constexpr int GameStart  = -4;
    static constexpr int Game       = -3;
    static constexpr int Pause      = -2;
    static constexpr int Result     = -1;
    static constexpr int Num        =  7;
};
struct CtrlID {
    static constexpr int Left      =  0; static constexpr int Right     =  1;
    static constexpr int CCWSpin   =  2; static constexpr int CWSpin    =  3;
    static constexpr int SoftDrop  =  4; static constexpr int HardDrop  =  5;
    static constexpr int ShowGhost =  6; static constexpr int ShowSkill =  7;
    static constexpr int Hold      =  8; static constexpr int Home      =  9;
    static constexpr int Restart   = 10; static constexpr int Init      = 11;
    static constexpr int Skip      = 12; static constexpr int Exit      = 13;
    static constexpr int Config    = 14; static constexpr int ChMusic   = 15;
    static constexpr int ChWindow  = 16; static constexpr int Mute      = 17;
    static constexpr int Pause     = 18; static constexpr int Start     = 19;
    static constexpr int Yes       = 20; static constexpr int No        = 21;
    static constexpr int Back      = 22; static constexpr int Forward   = 23;
    static constexpr int Down      = 24; static constexpr int Up        = 25;
    static constexpr int Num       = 26;
};
class Tetris {
public:
    Tetris();
    ~Tetris();
    void BackSystem();
    int DrawScene();
    int ClickBox(int x1, int y1, int x2, int y2);
    int HitCtrlKey(int CtrlNo);
private:
    int MakeMino(int x, int y, int Angle);
    int DisplayMino(int x, int y, int Kind, int Angle);
    int MakeBag();
    void NextMino();
    int DeleteMino();
    void ShapeNextMino(int MinoNo, int Kind);
    int ShapeMino(int x, int y, int[], int[], int Kind, int Angle);
    int ClearLine();
    int HoldMino();
    void AddMino();
    int ClockSpinMino();
    int AntiClockSpinMino();
    int MoveLeftMino();
    int MoveRightMino();
    int DropMino();
    int HardDropMino();
    int ShapeBottomMino();
    void ChangeWindow(int WindowModeFlag);
    bool ChangeKey(int x1, int y1, int x2, int y2, int CtrlNo);
    int HitKey(int keyCode);
    bool ChangeSound(int SoundModeFlag);
    bool ChangeSoundVol(int SoundNo, int Vol);
    bool PlayBGM(int BGMNo);
    struct IconID {
        static constexpr int Left          =  0; static constexpr int Right          =  1;
        static constexpr int CCWSpin       =  2; static constexpr int CWSpin         =  3;
        static constexpr int SoftDrop      =  4; static constexpr int HardDrop       =  5;
        static constexpr int Ghost         =  6; static constexpr int Skill          =  7;
        static constexpr int Hold          =  8; static constexpr int Home           =  9;
        static constexpr int Restart       = 10; static constexpr int Init           = 11;
        static constexpr int Skip          = 12; static constexpr int Exit           = 13;
        static constexpr int Config        = 14; static constexpr int Music          = 15;
        static constexpr int AnotherWindow = 16; static constexpr int MaximizeWindow = 17; 
        static constexpr int Mute          = 18; static constexpr int Unmute         = 19;
        static constexpr int Pause         = 20; static constexpr int Start          = 21;
        static constexpr int Back          = 22; static constexpr int Forward        = 23;
        static constexpr int Down          = 24; static constexpr int Up             = 25;
        static constexpr int Selected      = 26; static constexpr int Cursor         = 27;
        static constexpr int Num           = 28;
};
    int Icon[IconID::Num]; 
    const string CtrlName[17] = { "Left", "Right", "Anticlockwise", "Clockwise", "Soft", "Hard", 
                            "Ghost", "Skill",  "Hold", "Home", "Restart", "Initialize", "           Skip", 
                            "Quit Game", "Config", "Change BGM", "Change Window" };
    const string BoardCtrlName[4] = { "Move", "Spin", "Drop", "Show" };
    // Scene
    int Scene; const string SceneName[3] = { "Home", "Mode Select", "Char Select" };
    // BGM:1
    struct BGMID {
        static constexpr int TETRIS = 0;
        static constexpr int Chocolate = 1;
        static constexpr int DeadAlive = 2;
        static constexpr int Detect = 3;
        static constexpr int Hometown = 4;
        static constexpr int Legend = 5;
        static constexpr int NothingDies = 6;
        static constexpr int Sorrow = 7;
        static constexpr int Victory = 8;
        static constexpr int Num = 9;
    };
    int BGM[BGMID::Num], PlayingBGMNo, BGMVol;
    const string BGMName[BGMID::Num] = { "Theme of TETRIS", "Chocolate", "Dead or Alive", "Detect", "Hometown", "Memory of Legend",
                          "Nothing dies in my mind", "Sorrow", "Victory belongs to You" };
    struct BGMModeID {
        static constexpr int Loop = 0;
        static constexpr int Ascending = 1;
        static constexpr int Descending = 2;
        static constexpr int Random = 3;
        static constexpr int Num = 4;
    };
    const string BGMModeName[BGMModeID::Num] = { "Loop", "Ascending order", "Descending order", "Random" };
    struct SEID {
        static constexpr int Num = 4;
        static constexpr int ComboNum = 7;
    };
    int SE[SEID::Num], ComboSE[SEID::ComboNum], SEVol;
    struct MuteModeID {
        static constexpr int Mute = 0; static constexpr int Unmute = 1;
        static constexpr int Num = 2;
    };
    int MuteMode; const string MuteName[MuteModeID::Num] = { "Mute", "Unmute" }; 
    struct SoundID {
        static constexpr int Total = 0;
        static constexpr int BGM   = 1;
        static constexpr int SE    = 2;
        static constexpr int Num   = 3;
    };
    struct SoundVol {
        static constexpr int Min = 0;
        static constexpr int Max = 200;
    };
    const string SoundName[SoundID::Num] = { "Volume", " BGM", " SE" };
    struct BlockBoxData {
        static constexpr int W = 18;
        static constexpr int H = 40;
        static constexpr int BottomNo = 39;
    };
    // Block[x][y] = Shape ID of the Block on the Place
    struct MinoID {
        static constexpr int O       = 0; static constexpr int FixedO     =  8;
        static constexpr int I       = 1; static constexpr int FixedI     =  9;
        static constexpr int Z       = 2; static constexpr int FixedZ     = 10;
        static constexpr int T       = 3; static constexpr int FixedT     = 11;
        static constexpr int S       = 4; static constexpr int FixedS     = 12;
        static constexpr int L       = 5; static constexpr int FixedL     = 13;
        static constexpr int J       = 6; static constexpr int FixedJ     = 14;
        static constexpr int TypeNum = 7;
        static constexpr int Space   = 7; static constexpr int FixedSpace = 15;
        static constexpr int Num     = 8; static constexpr int BagNum = 14;
        static constexpr int Center  = 4; static constexpr int BlockNum = 4;
        static constexpr int PointNum = 5;
    };
    int BlockBox[BlockBoxData::W][BlockBoxData::H], BlockAngle, BlockShape[MinoID::Num], MinoBag[MinoID::BagNum], Index;
    // X-Y coordinate  X:Å®  Y:Å´ 
    int CursorX, CursorY, BlockX[MinoID::PointNum], BlockY[MinoID::PointNum], 
        CheckX[MinoID::BlockNum], CheckY[MinoID::BlockNum], BottomY[MinoID::PointNum];
    // imaginary number i^n = (CompX[n], CompY[n])
    const int CompX[4] = { 1, 0, -1, 0 }; const int CompY[4] = { 0, -1, 0, 1 };
    const int Ic[4] = { 0, 0, 1, 1 }; const int Im[4] = { -1, 0, 2, 1 }; const int Ie[4] = { 2, 0, -1, 1 };
    // indication of Ghost block & Skill name
    bool GhostMode, SkillMode; string SkillName[4] = { "   Single!", "  Double!!", "  Triple!!!", "!!TETRIS!!" };
    // Window
    int WindowMode;
    struct CharID {
        static constexpr int Initial = -13; static constexpr int Skip          = -7;
        static constexpr int John    =   0; static constexpr int BeforeJohn    = -6;
        static constexpr int Komachi =   1; static constexpr int BeforeKomachi = -5; 
        static constexpr int Misuzu  =   2; static constexpr int BeforeMisuzu  = -4;
        static constexpr int Michel  =   3; static constexpr int BeforeMichel  = -3;
        static constexpr int Yoichi  =   4; static constexpr int BeforeYoichi  = -2;
        static constexpr int Moscow  =   5; static constexpr int BeforeMoscow  = -1;
        static constexpr int Num     =   6;
    };
    int CharNo, CharLv, CharImage[CharID::Num], CharBGM[CharID::Num]; bool ExATK[CharID::Num];
    const string CharName[CharID::Num] = { "  John", "Komachi", " Misuzu", " Michel", " Yoichi", "Moscow"};
    const string CharRole[CharID::Num] = { "Racer", "Model", "Girl", "Witch", "Master", " ???"};
    const string CharFeature[CharID::Num] = { "He's fast!!\nExcept that,\nnormal...", "She bother you,\nso you can't\nsee next block!?",
                                    "Holded by her,\nyou can't hold!?", "She recites spell\nto add block!!",
                                    "He can use\nall skills!!", "The greatest will\nof Soviet\ncreated TETRIS"}; 
    // PlayMode
    struct ModeID {
        static constexpr int Initial  = -9; static constexpr int Skip = -5;
        static constexpr int Shuttle  =  0; static constexpr int BeforeShuttle  = -4;
        static constexpr int Marathon =  1; static constexpr int BeforeMarathon = -3;
        static constexpr int Sprint   =  2; static constexpr int BeforeSprint   = -2;
        static constexpr int Battle   =  3; static constexpr int BeforeBattle   = -1;
        static constexpr int Num      =  4;
    };
    int Mode; const string ModeName[ModeID::Num] = { "Shuttle", "Marathon", "Sprint", "Battle" };
    // Progress of Game
    struct PauseID {
        static constexpr int Resume = 0; static constexpr int Pause = 1;
        static constexpr int Num    = 2;
    };
    int Pause; const string PauseName[PauseID::Num] = { "Pause", "Resume" };
    // Game system
    int Turn, Shuttle, Phase, Score, Combo, CT, MaxCT, HP, MaxHP;
    static constexpr int MaxShuttle = 1361; static constexpr int MaxPhase = 65; 
    static constexpr int SprintLine =   40; static constexpr int MarathonLine = 150;
    int HoldCount, MoveCount, TotalLine, TotalTetris, TotalTspin, Spin, HoldState; bool BackToBack; 
    struct SpinID {
        static constexpr int Non = 0;
        static constexpr int Normal = 1;
        static constexpr int Special = 2;
    };
    int MoveTimeCount[2], FixTimeCount;  const int MoveTime = 3; const int FixTime = 30; double FallTime, FallTimeCount;
    const double FallAccel[MaxPhase] = { 0.0, 33.34, 16.66, 10.0, 6.7, 4.73, 3.57, 2.78, 2.22, 1.82, 1.52, 1.276, 1.104,
                                         0.947, 0.833, 0.736, 0.664, 0.574, 0.526, 3.334, 1.666, 1.0, 0.67, 0.473, 0.357,
                                         0.278, 0.222, 0.182, 0.152, 0.1276, 0.1104, 0.0947, 0.0833, 0.0736, 0.0664, 
                                         0.0574, 0.0526, 0.3334, 0.1666, 0.1, 0.067, 0.0473, 0.0357, 0.0278, 0.0222, 
                                         0.0182, 0.0152, 0.01276, 0.01104, 0.00947, 0.00833, 0.00736, 0.00664, 0.00574,
                                         0.00526, 0.0091, 0.00757, 0.00641, 0.0055, 0.00476, 0.00416, 0.00368, 0.00327, 
                                         0.00292, 0.00263 };
    const int KeyShuttle[MaxPhase + 1] = { 0, 7, 15, 23, 32, 41, 51, 61, 72, 83, 94, 106, 118, 131, 144, 157, 171, 185,
                                           200, 215, 231, 247, 263, 280, 297, 315, 333, 351, 370, 389, 409, 429, 450, 
                                           471, 492, 514, 536, 559, 582, 605, 629, 653, 678, 703, 729, 755, 781, 808, 
                                           835, 863, 891, 919, 948, 977, 1007, 1037, 1068, 1099, 1130, 1162, 1194, 1227,
                                           1260, 1293, 1327, 1361 };
    Timer t; static constexpr int StartCount = 3000;
    // Key
    const int KeyNo[KeyNum] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 28, 
                                29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 42, 44, 45, 46, 47, 48, 49, 50, 54, 56, 57, 58, 
                                59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 87, 88, 157, 184, 197, 200, 203, 205, 208, 210, 
                                211 };
    int KeyImage[MaxKeyNum]; 
    // Color
    Color c;
    unsigned int TitleColor, StrColor, FrameColor, HPColor, BlockColor[MinoID::Num + 1];
    // Font
    int NormalFont, TitleFont, CharNameFont;
    struct ConfigData {
        int SoundVol[SoundID::Num];              // SoundVol[SoundNo] SoundNo 0: Total, 1: BGM, 2: SE
        int BGMMode;                             // BGMStyle 0: Loop, 1: Ascending order, 2: Decending order, 3: Random" 
        int Key[CtrlID::Num];                    // Key[KeyRole]
        int TwinKey[CtrlID::Num];                // TwinKey[KeyRole]
        int SceneBGM[3];                         // SceneBGM[SceneNo]
        int MaxCharLv[ModeID::Num][CharID::Num]; // MaxCharLv[Mode][CharNo]
    }; ConfigData cd; 
    const char* DataFileName = "ConfigData.dat";
    // Function of each part
    void Config();
    void Home();
    void ModeSelect();
    void CharSelect();
    void GameStart();
    void InitScene();
    void Game();
    void UpdateScene(int TargetScene);
    int DrawCursor();
};
