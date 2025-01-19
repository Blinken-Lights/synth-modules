/*
* For Pi Pico
* CPU Speed: 250MHz (Overclock)
* Optimize: Optimize Even More (-O3)
*/

#include <RPi_Pico_TimerInterrupt.h>
#include <RPi_Pico_ISR_Timer.h>
#include <RPi_Pico_ISR_Timer.hpp>

#include <SPI.h>

#define BUFFERSIZE 7000

RPI_PICO_Timer ITimer0(0);

// TO DO:
// Move writing routine to core1 (maybe by moving the interrupt setup to setup1()) - will this allow a faster play speed / write speed?
// Test adding 4 more DACs
unsigned long dataLastRequested = 0;
int attemptReconnectAfter = 5000;

byte dacCss[] = { 28, 27, 26, 22, 21, 20, 17, 16 };
const int GAIN_1 = 0x1;
const int GAIN_2 = 0x0;

byte dacs[] =        { 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7 };
byte dacChannels[] = { 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1 };

byte digitalOutputs[] = { 8, 9, 10, 11, 12, 13, 14, 15 };

// buffer for digits
enum {BufSize=9};
char buf[BufSize];

unsigned long lastRefreshedDisplay = 0;
int refreshDisplayInterval = 17; // Just under 60Hz

String intBuffer = "";
byte recievedInt = 0;

volatile uint16_t testDataIn[BUFFERSIZE][16];

volatile int bufferWritePosition = 0;
volatile int bufferPlayPosition = 0;

volatile unsigned long absoluteReadingPosition = 0;
volatile unsigned long absoluteWritingPosition = 0;

volatile bool chunkSent = false;
int requestNewDataAt = 500;
volatile bool dataRequestSent = false;

void setup(void) {

  Serial.begin(2000000);
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB port only
  }

  establishContact();  // send a byte to establish contact until receiver responds

}

bool TimerHandler0(struct repeating_timer *t)
{

  if(absoluteReadingPosition >= absoluteWritingPosition){
    return true;
  }

  // One of these channels could double up as 12 flags for a set of gates,
  // although probably only about 8 pins are available on the Pico if the screen is removed
  for(byte channel = 0; channel < 16; channel++){
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
      if(bufferWritePosition >= BUFFERSIZE){
        bufferWritePosition = 0;
      }
    }
    if (inChar == '\t') {
      chunkSent = true;
      dataRequestSent = false;
    }
  }
}

void setup1(){
  SPI.begin();
  SPI.beginTransaction(SPISettings(25000000, MSBFIRST, SPI_MODE0));

  for(byte i = 0; i < 8; i++){
    pinMode(dacCss[i], OUTPUT);
  }

  for(byte i = 0; i < 8; i++){
    pinMode(digitalOutputs[i], OUTPUT);
  }
  
  ITimer0.attachInterruptInterval(600, TimerHandler0);
}

void loop(void) {

  if((chunkSent && !dataRequestSent && (absoluteWritingPosition - requestNewDataAt) <= absoluteReadingPosition)
    || (millis() - dataLastRequested > attemptReconnectAfter)){
    Serial.write("B");
    chunkSent = false;
    dataRequestSent = true;
    dataLastRequested = millis();
  }

}

void loop1(){


}

void setOutputChip1Channel(uint16_t val, byte channel, byte chip){
  byte lowByte = val & 0xff;
  byte highByte = ((val >> 8) & 0xff) | channel << 7 | (/*0x0*/0x1 << 5) | (1 << 4);

  digitalWrite(dacCss[chip], LOW);

  SPI.transfer(highByte);
  SPI.transfer(lowByte);

  digitalWrite(dacCss[chip], HIGH);
}