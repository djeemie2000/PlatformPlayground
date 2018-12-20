#pragma once

struct CommonState
{
    bool mutePressed;
    bool setPressed;
    bool clearPressed;
    int learnMode;
    bool playMode;//true = play mode, false = step mode

    bool clockIsRising;
    bool clockIsFalling;
    bool clockOn;
    int clockSegment;
    int clockNumSegments;
    
    float learnValue;

    int gateLengthChange;
};
