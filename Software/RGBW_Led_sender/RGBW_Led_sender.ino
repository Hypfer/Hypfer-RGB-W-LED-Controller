#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"


RF24 radio(7, 8);


const uint64_t broadcast = 0x00; //Basisaddresse

byte buf[5];

#define channel        0x4c                  // nrf24 communication channel
#define dataRate       RF24_250KBPS          // nrf24 data rate (lower == more distance)
#define paLevel        RF24_PA_HIGH          // nrf24 power level (black ebay models have some problems with PA_MAX)

void setup() {
  Serial.begin(57600);
  printf_begin();
  radio.begin();
  //radio.setDataRate(dataRate);
  radio.setPALevel(paLevel);
  radio.setChannel(channel);
  radio.openWritingPipe(broadcast);
  radio.enableDynamicAck();
  radio.setAutoAck(false);
  //Debug Info
  radio.printDetails();

  //sendCommand(2,0,0,0,16);

}

//set, slow Fade, fade, fast fade, pulsate, single Pulse
// 0,          1,      2    ,   3
#define MAX_MILLIS_TO_WAIT 1000  //or whatever
unsigned long starttime;
byte CMDin[6];
void loop() {




  starttime = millis();

  while ( (Serial.available() < 6) && ((millis() - starttime) < MAX_MILLIS_TO_WAIT) )
  {
    // hang in this loop until we either get 9 bytes of data or 1 second
    // has gone by
  }
  if (Serial.available() < 6)
  {
    // the data didn't come in - handle that problem here
    //Serial.println("ERROR - Didn't get 6 bytes of data!");
  }
  else
  {
    for (int n = 0; n < 6; n++) {
      CMDin[n] = Serial.read(); // Then: Get them.
    }
    sendCommand(CMDin[0], CMDin[1], CMDin[2], CMDin[3], CMDin[4], CMDin[5]);
  }
  /*
  Serial.write("Turned On");
    sendCommand(0,1,230,230,230,230);
    delay(1000);
    sendCommand(0,1,0,0,0,0);
    Serial.write("Turned Off");
    //delay(10000);
    //sendCommand(3,230,110,0,0);
    //delay(1800);
    // sendCommand(3,0,0,0,0);
    //delay(1500);

    //sendCommand(3,32,0,0,1);
    //delay(10000);
    //delay(3000);
    // Try again in a short while
    /*
    sendCommand(0,8,0,0,0);
    delay(1000);
    sendCommand(0,0,8,0,0);
    delay(1000);
    sendCommand(0,0,0,8,0);
    delay(1000);
    sendCommand(0,0,0,0,8);
    delay(1000);
    */
}

void sendCommand(byte addr, byte type, byte r, byte g, byte b, byte w) {
  //byte checksum = type + r + g + b + w;
  buf[0] = type;
  buf[1] = r;
  buf[2] = g;
  buf[3] = b;
  buf[4] = w;
  radio.openWritingPipe(broadcast + addr);

  radio.setAutoAck(false);
   //Erhöhen wenn nicht zuverlässig ankommen
  for (int i = 0; i <= 100; i++) {
    radio.write(&buf, 5, true );
  }

  //radio.write(&buf, 6, true );
}
