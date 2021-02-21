//Programa para generar señal PPM desde un arduino con una shield USB Host
//para controlar un coche de radiocontrol con el volante Logitech G27
//La señal PPM sale por el pin 2 del arduino hacia el puerto trainer de la emisora RC
//El volante controla el canal 1 de radiocontrol
//El acelerador controla el canal 2 del RC desde 1,5 ms hasta 2020 ms
//El freno controla el canal 2 del RC desde 1,5 hasta 0,98 ms
//El freno tiene prioridad sobre el acelerador

//Francisco Carabaza
//21/02/2021

//Este programa usa la siguiente librería:
//https://github.com/FabLabAQ/ArduinoG27

#include "ArduinoG27.h"
#define chanel_number 8  //set the number of chanels
#define PPM_FrLen 22500  //set the PPM frame length in microseconds (1ms = 1000µs) 8 canales 22500.
#define PPM_PulseLen 300  //set the pulse length
#define onState 1  //set polarity of the pulses: 1 is positive, 0 is negative
#define sigPin 2  //set PPM signal output pin on the arduino
#define MinPulse 980              //set the min usecs of pulse 
#define MaxPulse 2020             //set the max usecs of pulse
#define default_servo_value 1500  //set the default servo value
#define MinInput 0                //set the min input
#define MaxInput 1023             //set the max input

int steerWheel = 0;   //-8192 a 8191
int Throttle = 0;     //0 a 255
int Brake = 0;        //0 a 255
int ppm[chanel_number]; // Número de canales de la trama PPM
unsigned long time;

ArduinoG27 G27;

void setup()
{
  //Serial.begin(115200);
  G27.Init();

  //initiallize default ppm value
  ppm[0] = default_servo_value;
  ppm[1] = default_servo_value;
  ppm[2] = default_servo_value;
  ppm[3] = default_servo_value;
  ppm[4] = default_servo_value - 6;
  ppm[5] = default_servo_value - 6;
  ppm[6] = default_servo_value - 6;
  ppm[7] = default_servo_value - 6;

  pinMode(sigPin, OUTPUT);
  digitalWrite(sigPin, !onState);  //set the PPM signal pin to the default state (off)

  cli();
  TCCR1A = 0; // set entire TCCR1 register to 0
  TCCR1B = 0;
  OCR1A = 100;  // compare match register, change this
  TCCR1B |= (1 << WGM12);  // turn on CTC mode
  TCCR1B |= (1 << CS11);  // 8 prescaler: 0,5 microseconds at 16mhz
  TIMSK1 |= (1 << OCIE1A); // enable timer compare interrupt
  sei();

}


void loop()
{
  G27.Task();

  Throttle = (G27.GetThrottle());
  Brake = (G27.GetBrake());

  ppm[0] = (G27.GetWheelCentered());
  ppm[0] = map(ppm[0], -8192, 8191, MinPulse, MaxPulse);
  ppm[0] = constrain(ppm[0], MinPulse, MaxPulse);

  if (Brake > 1) {
    //ppm[1] = map(Brake, -255, 255, MinPulse, MaxPulse);
    ppm[1] = map(Brake, -255, 255, MaxPulse, MinPulse);
    ppm[1] = constrain(ppm[1], MinPulse, MaxPulse);
    //Serial.println(ppm[1]);
  }
  else if (Throttle > 1) {
    ppm[1] = map(Throttle, -255, 255, MinPulse, MaxPulse);
    ppm[1] = constrain(ppm[1], MinPulse, MaxPulse);
    //Serial.println(ppm[1]);
  }
  else {
    ppm[1] = default_servo_value;
  }
}

ISR(TIMER1_COMPA_vect) { //leave this alone
  static boolean state = true;
  TCNT1 = 0;
  if (state) { //start pulse
    digitalWrite(sigPin, onState);
    OCR1A = PPM_PulseLen * 2;
    state = false;
  }
  else { //end pulse and calculate when to start the next pulse
    static byte cur_chan_numb;
    static unsigned int calc_rest;
    digitalWrite(sigPin, !onState);
    state = true;
    if (cur_chan_numb >= chanel_number) {
      cur_chan_numb = 0;
      calc_rest = calc_rest + PPM_PulseLen;//
      OCR1A = (PPM_FrLen - calc_rest) * 2;
      calc_rest = 0;
    }
    else {
      OCR1A = (ppm[cur_chan_numb] - PPM_PulseLen) * 2;
      calc_rest = calc_rest + ppm[cur_chan_numb];
      cur_chan_numb++;
    }
  }
}
