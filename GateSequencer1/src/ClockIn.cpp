#include "ClockIn.h"


ClockIn::ClockIn()
  : period_(1024)
  , counter_(0)
  , prevValue_(false)
  , value_(false)
  , periodCount_(0)
{}


void ClockIn::tick(int clock){
  prevValue_ = value_;
  value_ = (clock!=0);
  if(isRising()) {
    // clock rising => start period
    period_ = counter_;//TODO smoothing over multiple periods
    counter_ = 0;
    ++periodCount_;
  } else {
    ++counter_;
  }
}
