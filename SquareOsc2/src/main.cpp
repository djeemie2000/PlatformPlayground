#include <mbed.h>

int main() {

  // put your setup code here, to run once:
  DigitalOut led(LED1);

  while(1) {
    // put your main code here, to run repeatedly:
    led.write(1);
    wait(0.4);
    led.write(0);
    wait(0.4);

    for(int repeat = 0; repeat<2; ++repeat)
    {
      led.write(1);
      wait(0.2);
      led.write(0);
      wait(0.2);
    }

  }
}

