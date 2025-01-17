/*
* For Pi Pico
* CPU Speed: 250MHz (Overclock)
* Optimize: Optimize Even More (-O3)
*
* In Ucglib.cpp, change both lines like
* SPI.beginTransaction(SPISettings(1000000000UL/((ucg_com_info_t *)data)->serial_clk_speed, MSBFIRST, SPI_MODE0));
* to
* SPI.beginTransaction(SPISettings(23000000, MSBFIRST, SPI_MODE0));
*/

#include <RPi_Pico_TimerInterrupt.h>
#include <RPi_Pico_ISR_Timer.h>
#include <RPi_Pico_ISR_Timer.hpp>

#include <SPI.h>
#include "Ucglib.h"
#include <PicoSPI.h>

#define BUFFERSIZE 14000

Ucglib_SSD1351_18x128x128_HWSPI ucg(/*cd=*/ 21, /*cs=*/ 17, /*reset=*/ 20);

RPI_PICO_Timer ITimer0(0);

//byte pwmOutputs[] = { 2, 3, 4, 5, 6, 7, 8, 9 };
byte dacCss[] = { 6, 7, 8, 9 };
const int GAIN_1 = 0x1;
const int GAIN_2 = 0x0;

byte dacs[] =        { 0, 0, 1, 1, 2, 2, 3, 3 };
byte dacChannels[] = { 0, 1, 0, 1, 0, 1, 0, 1 };

byte digitalOutputs[] = { 28, 27, 26, 22, 12, 13, 14, 15 };

// buffer for digits
enum {BufSize=9};
char buf[BufSize];

unsigned long lastRefreshedDisplay = 0;
int refreshDisplayInterval = 17; // Just under 60Hz

String intBuffer = "";
byte recievedInt = 0;

volatile uint16_t testDataIn[BUFFERSIZE][8];

volatile int bufferWritePosition = 0;
volatile int bufferPlayPosition = 0;

volatile unsigned long absoluteReadingPosition = 0;
volatile unsigned long absoluteWritingPosition = 0;

int pausePlayAt = 250;
volatile bool chunkSent = false;
int requestNewDataAt = 500; // at a play speed of 1000Hz, this will ask for new data from Processing when only 2s remains in the buffer
volatile bool dataRequestSent = false;
/*int pausePlayAt = 250; // should always be less than requestNewDataAt
volatile bool askForMoreData = true;
//volatile unsigned long dataRequestSentAt = 0;
volatile bool prevAskForMoreData = false;
volatile int unsetAskFlagAfter = 250;
volatile int dataReceivedAfterAsked = 0;*/

void setup(void) {

  Serial.begin(2000000);
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB port only
  }

  establishContact();  // send a byte to establish contact until receiver responds

  if (! PicoSPI1.configure (10, 11, 8, 9, 25000000, 0, false))
  {
    while (true) {}
  }

  for(byte i = 0; i < 4; i++){
    pinMode(dacCss[i], OUTPUT);
  }

  for(byte i = 0; i < 8; i++){
    pinMode(digitalOutputs[i], OUTPUT);
  }
  
  ITimer0.attachInterruptInterval(350, TimerHandler0);

}

bool TimerHandler0(struct repeating_timer *t)
{

  //if(absoluteReadingPosition > absoluteWritingPosition - pausePlayAt){
  if(absoluteReadingPosition >= absoluteWritingPosition){
    /*if(!dataRequestSent){
      askForMoreData = true;
      dataReceivedAfterAsked = 0;
    }*/
    return true;
  }

  // One of these channels could double up as 12 flags for a set of gates,
  // although probably only about 8 pins are available on the Pico if the screen is removed
  for(byte channel = 0; channel < 8; channel++){
    setOutputChip1Channel(testDataIn[bufferPlayPosition][channel], dacChannels[channel], dacs[channel]);
  }

  // Also interpret channel 0 as a series of bit flags
  for(byte i = 0; i < 8; i++){
    digitalWrite( digitalOutputs[i], bitRead(testDataIn[bufferPlayPosition][0], i) );
  }

  bufferPlayPosition++;
  absoluteReadingPosition++;
  if(bufferPlayPosition >= BUFFERSIZE){
    bufferPlayPosition = 0;
  }

  /*if(!dataRequestSent && absoluteWritingPosition - absoluteReadingPosition >= requestNewDataAt){
    askForMoreData = true;
    dataReceivedAfterAsked = 0;
  }*/

  return true;
}

// just for testing?
void establishContact() {
  while (Serial.available() <= 0) {
    Serial.write("B");  // send an initial string
    delay(300);
  }
}

/*
  SerialEvent occurs whenever a new data comes in the hardware serial RX. This
  routine is run between each time loop() runs, so using delay inside loop can
  delay response. Multiple bytes of data may be available.
*/
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();

    if(inChar == ','){
      /*while(absoluteReadingPosition > BUFFERSIZE && absoluteWritingPosition < (absoluteReadingPosition - 2000)){
        // wait for play position to catch up
      }*/
      testDataIn[bufferWritePosition][recievedInt] = intBuffer.toInt();
      intBuffer = "";
      recievedInt++;
    }else{
      intBuffer += inChar;
    }

    if (inChar == '\n') {
      recievedInt = 0;
      intBuffer = "";
      bufferWritePosition++;
      absoluteWritingPosition++;
      /*dataReceivedAfterAsked++;
      if(dataReceivedAfterAsked > unsetAskFlagAfter){
        dataRequestSent = false;
      }*/
      if(bufferWritePosition >= BUFFERSIZE){
        bufferWritePosition = 0;
      }
    }
    if (inChar == '\t') {
      //Serial.write("B");
      chunkSent = true;
      dataRequestSent = false;
    }
  }
}

void setup1(){
  ucg.begin(UCG_FONT_MODE_SOLID);
  ucg.setFont(ucg_font_7x13_mr);
  ucg.setColor(0, 255, 255, 255);
  ucg.setColor(1, 0, 0, 0);
  ucg.clearScreen();
}

void loop(void) {

  //if(chunkSent && !dataRequestSent && absoluteWritingPosition - absoluteReadingPosition >= requestNewDataAt){
  if(chunkSent && !dataRequestSent && (absoluteWritingPosition - requestNewDataAt) <= absoluteReadingPosition){
    Serial.write("B");
    chunkSent = false;
    dataRequestSent = true;
  }

  /*if(askForMoreData && !prevAskForMoreData){
    Serial.write("B");
    askForMoreData = false;
    dataRequestSent = true;
  }

  prevAskForMoreData = askForMoreData;*/

}

// Use 2nd core for display. Otherwise the interrupt for the DACs will slow it down
void loop1(){

  for(int i = 0; i < 8; i++){
    ucg.setPrintPos(0, 12 + (12 * i));
    ucg.print("           ");
    ucg.setPrintPos(0, 12 + (12 * i));
    ucg.print(testDataIn[bufferWritePosition][i]);
  }

  delay(100);

}

void setOutputChip1Channel(uint16_t val, byte channel, byte chip){
  byte lowByte = val & 0xff;
  byte highByte = ((val >> 8) & 0xff) | channel << 7 | (/*0x0*/0x1 << 5) | (1 << 4);

  digitalWrite(dacCss[chip], LOW);

  PicoSPI1.transfer(highByte);
  PicoSPI1.transfer(lowByte);

  digitalWrite(dacCss[chip], HIGH);
}
