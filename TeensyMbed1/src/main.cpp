#include <mbed.h>

int main() {

  // put your setup code here, to run once:
  Serial pc(D1, D0);// TX1 RX1
  pc.baud(115200);

  DigitalOut led(LED1);
  PwmOut pwm1(D23);
  pwm1.period_us(125);//8kHz


  pc.printf("Teensy mbed ...\r\n");

  //PwmOut led(LED1);
  //led.period_ms(500);
  //led.write(0.10);//30% duty cycle

  // PwmOut fastPwm1(A1);
  // fastPwm1.period_us(100);//10 kHz
  // fastPwm1.write(0.20);//20% duty cycle
  //
  // PwmOut fastPwm2(A2);
  // fastPwm1.period_us(100);//10 kHz
  // fastPwm1.write(0.40);//40% duty cycle

  int counter = 0;
  while(1) {
    wait_ms(1000);

    led = !led;

    if(led) {
      pwm1.write(0.80);//80%
    } else {
      pwm1.write(0.10);//1s0%
    }

    ++counter;
    int tmp = led;
    pc.printf("Count %d %d \r\n", counter, tmp);
  }

}
