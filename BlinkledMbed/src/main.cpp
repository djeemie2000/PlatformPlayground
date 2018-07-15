//#include "stm32f103c8t6.h"
#include <mbed.h>

const PinName ledPin = PC_13;
//const PinName outputPin = PA_0;

DigitalOut ledOut(ledPin);
//PwmOut pwmOut(outputPin);
Serial pc(PA_2, PA_3); // tx, rx
Serial pc2(PA_9, PA_10); // tx, rx
Serial pc3(USBTX, USBRX);

int counter;

void ledOn() {
  ledOut = 1;
  //pwmOut.pulsewidth_us(800);
  ++counter;
  pc.printf("Led on %d\r\n", counter);
  pc2.printf("Led on %d\r\n", counter);
  pc3.printf("Led on %d\r\n", counter);
}

void ledOff() {
  ledOut = 0;
  //pwmOut.pulsewidth_us(300);
  pc.printf("Led off\r\n");
  pc2.printf("Led off\r\n");
  pc3.printf("Led off\r\n");
}

int main() {

    // put your setup code here, to run once:
    //confSysClock();     //Configure system clock (72MHz HSE clock, 48MHz USB clock)
    //SetSysClock();

    //pwmOut.period_ms(1);
    pc.baud(115200);
    pc.printf("BlinkLed...");
    pc2.baud(115200);
    pc2.printf("BlinkLed...");
    pc3.baud(115200);
    pc3.printf("BlinkLed...");
    counter = 0;


    while(1) {
        // put your main code here, to run repeatedly:
        for(int repeat = 0; repeat<8; ++repeat) {
          ledOn();
          wait_ms(250);
          ledOff();
          wait_ms(250);
        }

        ledOn();
        wait_ms(1000);
        ledOff();
        wait_ms(1000);
    }
}
