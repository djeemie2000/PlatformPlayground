#pragma once

struct CommonState
{
    bool mutePressed;
    bool setPressed;
    bool clearPressed;
    bool resetStepPressed;

    bool selectPatternPressed;
    bool selectBankPressed;

    bool savePatternsPressed;
    //TODO change step duration button??

    bool forceAdvanceOn;
    bool forceAdvanceOff;
    
    int clockCntr;
    int clockPeriod;
};
