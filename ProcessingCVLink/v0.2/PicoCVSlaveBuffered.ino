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

bool core1_separate_stack = true;

RPI_PICO_Timer ITimer0(0);

unsigned long dataLastRequested = 0;
int attemptReconnectAfter = 5000;

byte dacCss[] = { 28, 27, 26, 22, 21, 20, 17, 16 };
const int GAIN_1 = 0x1;
const int GAIN_2 = 0x0;

byte dacs[] =        { 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7 };
byte dacChannels[] = { 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0 }; //{ 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1 };

byte digitalOutputs[] = { 15, 14, 13, 12, 11, 10, 9, 8 }; // { 8, 9, 10, 11, 12, 13, 14, 15 };

// buffer for digits
enum {BufSize=9};
char buf[BufSize];

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

volatile bool ledState = false;

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
    digitalWrite(LED_BUILTIN, HIGH);
    return true;
  }

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
      // TO DO: add a 'start' character, or data length check, to make sure that only valid numbers are saved to the buffer??
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
      digitalWrite(LED_BUILTIN, LOW);
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

  pinMode(LED_BUILTIN, OUTPUT);
  
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

  val = val | channel << 15 | 12288;

  digitalWrite(dacCss[chip], LOW);

  SPI.transfer16(val);

  digitalWrite(dacCss[chip], HIGH);
}