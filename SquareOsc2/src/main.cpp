#include <mbed.h>

DigitalOut led(LED1);
Ticker ticker;

DigitalIn gateIn(PB_14, PullUp);
DigitalOut oscOut(PB_15);
uint32_t oscPeriod;
uint32_t oscCntr;

void ledOff()
{
  led = 0;
}

void ledOn()
{
  led = 1;
}

void toggleOsc()
{
  int gate = gateIn;
  ++oscCntr;
  if(oscCntr >= oscPeriod)
  {
    oscCntr = 0;
    if(gate)
    {
      oscOut = !oscOut;
    }
    else 
    {
      oscOut = 0;
    }
  }
}

int main() {

  // put your setup code here, to run once:
  oscPeriod = 1;
  oscCntr = 0;

  us_timestamp_t period_us = 200;// 5KHz
  ticker.attach_us(toggleOsc, period_us);

  while(1) {
    //put your main code here, to run repeatedly:
    ledOn();
    wait(0.7);
    ledOff();
    wait(0.7);
    ++oscPeriod;
    if(oscPeriod>16)
    {
      oscPeriod = 1;
    }
  }
}

