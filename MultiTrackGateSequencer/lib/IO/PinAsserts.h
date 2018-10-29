#pragma once

#include <mbed.h>
#include <type_traits>

//for now, only blue pill platform!!!
namespace pinasserts
{

bool IsPwm(PinName pin)
{
    return pin == PA_8 ||
            pin == PA_9 ||
            pin == PA_10 ||
            pin == PB_6 ||
            pin == PB_7 ||
            pin == PB_8 ||
            pin == PB_9 ||
            pin == PA_0 ||
            pin == PA_1 ||
            pin == PA_2 ||
            pin == PA_3 ||
            pin == PA_6 ||
            pin == PA_7 ||
            pin == PB_0 ||
            pin == PB_1;
}

}
