#pragma once

class xbox_input
{
public:
    static void Initialize();
    static void ScanPads();
    static void Clear();

    static bool Exit();
    static bool Pause();
    static bool NewGame();

    static bool LaunchBallDown();
    static bool LaunchBallUp();

    static bool MoveLeftPaddleDown();
    static bool MoveLeftPaddleUp();
    static bool MoveRightPaddleDown();
    static bool MoveRightPaddleUp();

    static bool NudgeLeftDown();
    static bool NudgeLeftUp();
    static bool NudgeRightDown();
    static bool NudgeRightUp();
    static bool NudgeUpDown();
    static bool NudgeUpUp();

    static bool Button1();
    static bool Button2();

private:
    static unsigned int xboxButtonsDown;
    static unsigned int xboxButtonsUp;
    static unsigned int xboxButtonsHeld;
    static bool xboxLeftTriggerDown;
    static bool xboxLeftTriggerUp;
    static bool xboxRightTriggerDown;
    static bool xboxRightTriggerUp;
};
