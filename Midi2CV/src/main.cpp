#include <Arduino.h>
#include <Wire.h>

#include "ScanI2C.h"
#include "mcp4728.h"

const int pwmPin = 5;
int clockShift;
MCP4728Dac Dac1;

// void setupFastPwm()
// {
//   Serial.println("Setup fast pwm");
//   // ... TODO ...

//   //---------------------------------------------- Set PWM frequency for D5 & D6 -------------------------------

//   // TCCR0B = TCCR0B & B11111000 | B00000001; // set timer 0 divisor to 1 for PWM frequency of 62500.00 Hz
//   TCCR0B = TCCR0B & B11111000 | B00000010; // set timer 0 divisor to 8 for PWM frequency of 7812.50 Hz
//   // TCCR0B = TCCR0B & B11111000 | B00000011; // set timer 0 divisor to 64 for PWM frequency of 976.56 Hz
//   // TCCR0B = TCCR0B & B11111000 | B00000100; // set timer 0 divisor to 256 for PWM frequency of 244.14 Hz
//   // TCCR0B = TCCR0B & B11111000 | B00000101; // set timer 0 divisor to 1024 for PWM frequency of 61.04 Hz

//   clockShift = 8;

//   // Pins D5 and D6 - 7.8 kHz
//   // TCCR0B = 0b00000010; // x8
//   // TCCR0A = 0b00000011; // fast pwm

//   //---------------------------------------------- Set PWM frequency for D9 & D10 ------------------------------

//   // TCCR1B = TCCR1B & B11111000 | B00000001; // set timer 1 divisor to 1 for PWM frequency of 31372.55 Hz
//   // TCCR1B = TCCR1B & B11111000 | B00000010; // set timer 1 divisor to 8 for PWM frequency of 3921.16 Hz
//   TCCR1B = TCCR1B & B11111000 | B00000011; // set timer 1 divisor to 64 for PWM frequency of 490.20 Hz
//   // TCCR1B = TCCR1B & B11111000 | B00000100; // set timer 1 divisor to 256 for PWM frequency of 122.55 Hz
//   // TCCR1B = TCCR1B & B11111000 | B00000101; // set timer 1 divisor to 1024 for PWM frequency of 30.64 Hz
// }

// void setupNormalPwm()
// {
//   Serial.println("Setup normal pwm");
//   // ... TODO ...

//   //---------------------------------------------- Set PWM frequency for D5 & D6 -------------------------------

//   // TCCR0B = TCCR0B & B11111000 | B00000001; // set timer 0 divisor to 1 for PWM frequency of 62500.00 Hz
//   // TCCR0B = TCCR0B & B11111000 | B00000010; // set timer 0 divisor to 8 for PWM frequency of 7812.50 Hz
//   TCCR0B = TCCR0B & B11111000 | B00000011; // set timer 0 divisor to 64 for PWM frequency of 976.56 Hz
//   // TCCR0B = TCCR0B & B11111000 | B00000100; // set timer 0 divisor to 256 for PWM frequency of 244.14 Hz
//   // TCCR0B = TCCR0B & B11111000 | B00000101; // set timer 0 divisor to 1024 for PWM frequency of 61.04 Hz

//   clockShift = 0;

//   // Pins D5 and D6 - 7.8 kHz
//   // TCCR0B = 0b00000010; // x8
//   // TCCR0A = 0b00000011; // fast pwm

//   //---------------------------------------------- Set PWM frequency for D9 & D10 ------------------------------

//   // TCCR1B = TCCR1B & B11111000 | B00000001; // set timer 1 divisor to 1 for PWM frequency of 31372.55 Hz
//   // TCCR1B = TCCR1B & B11111000 | B00000010; // set timer 1 divisor to 8 for PWM frequency of 3921.16 Hz
//   TCCR1B = TCCR1B & B11111000 | B00000011; // set timer 1 divisor to 64 for PWM frequency of 490.20 Hz
//   // TCCR1B = TCCR1B & B11111000 | B00000100; // set timer 1 divisor to 256 for PWM frequency of 122.55 Hz
//   // TCCR1B = TCCR1B & B11111000 | B00000101; // set timer 1 divisor to 1024 for PWM frequency of 30.64 Hz
// }

// void TestPWM()
// {
//   // fast = 1-fast;
//   // if(fast)
//   // {
//   //   setupFastPwm();
//   // }
//   // else
//   // {
//   //   setupNormalPwm();
//   // }

//   Serial.println("Test pwm");
//   // 8 bit resultion [0,255]
//   const int delayPeriod = 800 << clockShift;
//   analogWrite(pwmPin, 0);
//   delay(delayPeriod);
//   analogWrite(pwmPin, 256 / 5);
//   delay(delayPeriod);
//   analogWrite(pwmPin, 0);
//   delay(delayPeriod);
//   analogWrite(pwmPin, 2 * 256 / 5);
//   delay(delayPeriod);
//   analogWrite(pwmPin, 0);
//   delay(delayPeriod);
//   analogWrite(pwmPin, 3 * 256 / 5);
//   delay(delayPeriod);
//   analogWrite(pwmPin, 0);
//   delay(delayPeriod);
//   analogWrite(pwmPin, 4 * 256 / 5);
//   delay(delayPeriod);
// }

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200); // TODO midi Rx/Tx

  Serial.println("Midi2CV poc ...");

  // PWM out -> RC filter 104 47k -> buffer opamp
  // setupNormalPwm();

  Dac1.Begin(MCP4728Dac::MCP4728_I2CADDR_DEFAULT);
  // pitch channels : internal ref + gain x2 => 0-4.096 V
  // velocity channels : VDD (5V) ref + gain x1 => 0-5V
  Dac1.ConfigureChannel(0, MCP4728Dac::vref::MCP4728_VREF_INTERNAL, MCP4728Dac::gain::MCP4728_GAIN_2X);
  Dac1.ConfigureChannel(1, MCP4728Dac::vref::MCP4728_VREF_VDD, MCP4728Dac::gain::MCP4728_GAIN_1X);
  Dac1.ConfigureChannel(2, MCP4728Dac::vref::MCP4728_VREF_INTERNAL, MCP4728Dac::gain::MCP4728_GAIN_2X);
  Dac1.ConfigureChannel(3, MCP4728Dac::vref::MCP4728_VREF_VDD, MCP4728Dac::gain::MCP4728_GAIN_1X);
}

void TestDac()
{
  // test Dac
  const int delayPeriod = 400;
  for (int ch = 0; ch < MCP4728Dac::NumChannels; ++ch)
  {
    // 12 bits value
    for (int value = 0; value < 4096; value += 500)
    {
      Dac1.SetValue(ch, value);
      Dac1.Update();

      Serial.print("ch ");
      Serial.print(ch);
      Serial.print(" : ");
      Serial.println(value);

      delay(delayPeriod);
    }
  }
}

void loop()
{
  // put your main code here, to run repeatedly:

  ScanI2C(Serial);

  TestDac();
}
