#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"
#include <LEDFader.h>
#include <Curve.h>

//uses https://github.com/jgillick/arduino-LEDFader

RF24 radio(7, 8);

const uint64_t NODE_ID = 1;

const uint64_t broadcast = 0x00; //Basisaddresse
const uint64_t address = broadcast + NODE_ID;

#define channel        0x4c                  // nrf24 communication channel
#define dataRate       RF24_250KBPS          // nrf24 data rate (lower == more distance)
#define paLevel        RF24_PA_HIGH          // nrf24 power level (black ebay models have some problems with PA_MAX)

byte buf[4];


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
  //radio.setDataRate(dataRate);
  radio.setPALevel(paLevel);
  radio.setChannel(channel);
  radio.openReadingPipe(0, address);
  radio.openReadingPipe(1, broadcast);
  radio.enableDynamicAck();
  radio.setAutoAck(false);
  radio.startListening();

  // Debug Info
  radio.printDetails();

  //LED Channel Initialisieren
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

void loop() {
  while (radio.available())
  {
    r.update();
    g.update();
    b.update();
    w.update();
    radio.read(&buf, 5);


    //set, slow Fade, fade, fast fade, pulsate, single Pulse
    // 0,          1,      2    ,   3

    Serial.println(buf[1]);
    Serial.println(buf[2]);
    Serial.println(buf[3]);
    Serial.println(buf[4]);
    switch (buf[0]) {
      case 0:
        r.fade(buf[1], 0);
        g.fade(buf[2], 0);
        b.fade(buf[3], 0);
        w.fade(buf[4], 0);
        break;

      case 1:
        r.fade(buf[1], 5000);
        g.fade(buf[2], 5000);
        b.fade(buf[3], 5000);
        w.fade(buf[4], 5000);
        break;
      case 2:
        r.fade(buf[1], 3000);
        g.fade(buf[2], 3000);
        b.fade(buf[3], 3000);
        w.fade(buf[4], 3000);
        break;
      case 3:
        r.fade(buf[1], 1000);
        g.fade(buf[2], 1000);
        b.fade(buf[3], 1000);
        w.fade(buf[4], 1000);
        break;
        //Buf nullen? gegen unerwartete Ergebnisse
    }
  }
  r.update();
  g.update();
  b.update();
  w.update();
}


