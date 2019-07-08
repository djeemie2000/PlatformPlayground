#include <Arduino.h>

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("BlinkLed v0.1...");

  pinMode(LED_BUILTIN, OUTPUT);
}

void BlinkLed(int period)
{
  digitalWrite(LED_BUILTIN,1);
  Serial.println("Led ON");
  delay(period);
  digitalWrite(LED_BUILTIN,0);
  Serial.println("Led OFF");
  delay(period);
}

void loop() {
  // put your main code here, to run repeatedly:
  BlinkLed(300);
  BlinkLed(300);
  BlinkLed(800);
}

