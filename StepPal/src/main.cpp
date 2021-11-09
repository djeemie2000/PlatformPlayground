#include <Arduino.h>
#include <SPI.h>

#include "cvclock.h"
#include "summingquantizer.h"
#include "mcp48xxdac.h"

struct StepPalApp
{
public:
  static const int NumCvClock = 4;
  static const int NumSummingQuantizer = 2;

  CVClock cvClock[NumCvClock];
  SummingQuantizer summingQuantizer[NumSummingQuantizer];

  void begin()
  {
    mcp48xxdac::Begin(1);// 1x 2-channel SPI DAC

    cvClock[0].Begin(3, 7, A0);
    cvClock[0].Begin(4, 8, A1);
    cvClock[0].Begin(5, 9, A2);
    cvClock[0].Begin(6, 10, A3);

    summingQuantizer[0].begin(A4, A5, 0);
    summingQuantizer[1].begin(A6, A7, 1);
  }

  void update()
  {
    cvClock[0].Tick();
    cvClock[1].Tick();
    cvClock[2].Tick();
    cvClock[3].Tick();

    summingQuantizer[0].update();
    summingQuantizer[1].update();

    delay(1);//needed?
  }
};

StepPalApp app;

void setup() {
  // put your setup code here, to run once:
  app.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  app.update();
}
