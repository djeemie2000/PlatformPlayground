#pragma once

struct CommonState
{
    bool mutePressed;
    bool setPressed;
    bool clearPressed;
    bool resetStepPressed;
    
    // int learnMode;
    // bool playMode;//true = play mode, false = step mode

    bool clockIsRising;
    bool clockIsFalling;
    bool clockOn;
    int clockCntr;
    int clockPeriod;
    
    // float learnValue;
};
