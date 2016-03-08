#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"
#include <LEDFader.h>
#include <Curve.h>

//uses https://github.com/jgillick/arduino-LEDFader

RF24 radio(7, 8);

/*
Bad    1
Flur   2
Schlaf 3
Küche  4
Wohn   5
*/

const uint64_t NODE_ID = 2;

const uint64_t securityOffset = 1337; //"Security" by pretending
const uint8_t broadcast = 0x00; //Base Addr
const uint64_t address = broadcast + NODE_ID;

#define channel        0x4c                  // nrf24 communication channel
#define paLevel        RF24_PA_HIGH          // nrf24 power level (black ebay models have some problems with PA_MAX)

byte buf[5];


LEDFader r = LEDFader(3);
LEDFader g = LEDFader(9);
LEDFader b = LEDFader(10);
LEDFader w = LEDFader(5);



void setup() {
  //GOTTA GO FAST
  TCCR1B = TCCR1B & B11111000 | B00000001;
  TCCR2B = TCCR2B & B11111000 | B00000001;


  Serial.begin(57600);
  printf_begin();
  radio.begin();
  radio.setPALevel(paLevel);
  radio.setChannel(channel);
  radio.openReadingPipe(0, securityOffset + address);
  radio.openReadingPipe(1, securityOffset + broadcast);
  radio.enableDynamicAck();
  radio.setAutoAck(false);
  radio.startListening();

  //Init LEDs
  pinMode(3, OUTPUT); //R
  analogWrite(3, 0);
  pinMode(9, OUTPUT); //G
  analogWrite(9, 0);
  pinMode(10, OUTPUT); //B
  analogWrite(10, 0);
  pinMode(5, OUTPUT); //W
  analogWrite(5, 0);

  r.set_curve(&Curve::exponential);
  g.set_curve(&Curve::exponential);
  b.set_curve(&Curve::exponential);
  w.set_curve(&Curve::exponential);
}

void fade(int delay) {
  r.fade(buf[1], delay);
  g.fade(buf[2], delay);
  b.fade(buf[3], delay);
  w.fade(buf[4], delay);
}

void loop() {
  while (radio.available())
  {
    r.update();
    g.update();
    b.update();
    w.update();
    radio.read(&buf, 5);

    //set, slow Fade, fade, fast fade
    // 0,          1,    2,   3
    switch (buf[0]) {
      case 0:
        fade(0);
        break;
      case 1:
        fade(5000);
        break;
      case 2:
        fade(3000);
        break;
      case 3:
        fade(1000);
        break;
        //Buf nullen? gegen unerwartete Ergebnisse
    }
  }
  r.update();
  g.update();
  b.update();
  w.update();
}

