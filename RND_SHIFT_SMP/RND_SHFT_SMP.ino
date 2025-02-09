#include <MCP48xx.h>

MCP4822 dac1(9);
MCP4822 dac2(10);

const byte quantised[] PROGMEM = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 60, 60, 60, 60, 60, 60, 60, 60, 60 };

byte triggerPins[] = { 12, 2, 3, 4 };

bool prevTriggerStates[] = { false, false, false, false };

byte analoguePins[] = { A7, A6, A5, A4 };

int prevAnalogueReadings[] = { -1, -1, -1, -1 };
byte jitterThreshold = 4;

byte swPins[4][2] = {
  { A3, 5 },
  { A2, 6 },
  { A1, 7 },
  { A0, 8 }
};

byte possibleAnalogueRandomPins[] = { A0, A1, A2, A3 };

byte dacs[] = { 1, 1, 0, 0 };
byte dacChannels[] = { 1, 0, 0, 1 };

byte currentOutputValues[] = { 0, 0, 0, 0 };
byte prevOutputValues[] = { 0, 0, 0, 0 };

void setup() {

  dac1.init();
  dac2.init();

  // The channels are turned off at startup so we need to turn the channel we need on
  dac1.turnOnChannelA();
  dac1.turnOnChannelB();
  dac2.turnOnChannelA();
  dac2.turnOnChannelB();

  // We configure the channels in High gain
  // It is also the default value so it is not really needed
  dac1.setGainA(MCP4822::High);
  dac1.setGainB(MCP4822::High);
  dac2.setGainA(MCP4822::High);
  dac2.setGainB(MCP4822::High);

  for(int i = 0; i < 4; i++){
    quantisedOutput(i, 0);
  }

  // try to find an analogue pin that isn't connected to GND via a switch
  for (int i = 0; i < 4; i++) {
    int randomAnalogue = analogRead(possibleAnalogueRandomPins[i]);

    if (randomAnalogue < 10) {
      continue;
    }

    randomSeed(randomAnalogue);
  }

  for (int i = 0; i < 4; i++) {
    pinMode(triggerPins[i], INPUT);
    pinMode(analoguePins[i], INPUT);
    for (int j = 0; j < 2; j++) {
      pinMode(swPins[i][j], INPUT_PULLUP);
    }
  }

  for (int i = 0; i < 4; i++) {
    prevAnalogueReadings[i] = analogRead(analoguePins[i]);
  }
}

void loop() {

  for (int i = 0; i < 4; i++) {

    bool triggerState = opt_read(triggerPins[i]);

    if (triggerState && !prevTriggerStates[i]) {

      prevOutputValues[i] = currentOutputValues[i];

      byte swState = (!opt_read(swPins[i][0]) << 1) | !opt_read(swPins[i][1]);

      if (swState == 0) {
        // Sample
        int analogReading = analogRead(analoguePins[i]);
        if (abs(prevAnalogueReadings[i] - analogReading) > jitterThreshold) {
          currentOutputValues[i] = quantise(analogReading);
          prevAnalogueReadings[i] = analogReading;
          quantisedOutput(i, currentOutputValues[i]);
        }
        checkForShiftDown(i);
      }else if(swState == 1) {
        // Random Sample
        currentOutputValues[i] = random(0, 61);
        quantisedOutput(i, currentOutputValues[i]);
        checkForShiftDown(i);
      }else if(swState == 2) {
        // shift in value from channel above it
        // (can loop values if all channel switches are set to 2 = Shift)
        byte previousChannel = i - 1;
        if(previousChannel < 0){
          previousChannel = 3;
        }
        currentOutputValues[i] = currentOutputValues[previousChannel];
        quantisedOutput(i, currentOutputValues[i]);
        checkForShiftDown(i);
      }
    }

    prevTriggerStates[i] = triggerState;
  }
}

void checkForShiftDown(byte startingChannel) {

  byte shiftAlongNote = prevOutputValues[startingChannel];
  byte writeNote = prevOutputValues[startingChannel];

  for (int i = 0; i < 3; i++) {

    byte checkChannel = i + startingChannel + 1;
    if (checkChannel >= 4) {
      checkChannel -= 4;
    }

    byte swState = (!opt_read(swPins[checkChannel][0]) << 1) | !opt_read(swPins[checkChannel][1]);

    if (swState != 2) {
      return;
    }

    writeNote = shiftAlongNote;
    shiftAlongNote = currentOutputValues[checkChannel];
    currentOutputValues[checkChannel] = writeNote;

    quantisedOutput(checkChannel, currentOutputValues[checkChannel]);
  }
}

byte quantise(int analogValue) {

  return pgm_read_byte_near(quantised + analogValue);

}

void quantisedOutput(byte channel, byte note) {

  if (note > 60) {
    note = 60;
  }

  // at note 60, output of DAC should be (60 * 68) = 4080
  // output of scaled op amp should be 5V
  // Ideal scale factor (5000 / 4080) = 1.225 . Resistors: 100k to GND from INV, 22k + 510R from INV to OUT

  setOutputChip1Channel(note * 68, dacChannels[channel], dacs[channel]);
}

void setOutputChip1Channel(int val, byte channel, byte chip) {

  if(chip == 0){
    if(channel == 0){
      dac1.setVoltageA( val );
    }else if(channel == 1){
      dac1.setVoltageB( val );
    }
    dac1.updateDAC();
  }else if(chip == 1){
    if(channel == 0){
      dac2.setVoltageA( val );
    }else if(channel == 1){
      dac2.setVoltageB( val );
    }
    dac2.updateDAC();
  }

}

bool opt_read(byte pin) {
  if (pin >= 14) {
    pin -= 14;
    return bitRead(PINC, pin);
  } else if (pin >= 8) {
    pin -= 8;
    return bitRead(PINB, pin);
  } else {
    return bitRead(PIND, pin);
  }
}

void opt_write(byte pin, bool val) {
  if (pin >= 14) {
    pin -= 14;
    bitWrite(PORTC, pin, val);
  } else if (pin >= 8) {
    pin -= 8;
    bitWrite(PORTB, pin, val);
  } else {
    bitWrite(PORTD, pin, val);
  }
}

void opt_mode(byte pin, byte val) {
  if (val == 2) {
    opt_write(pin, HIGH);
    val = 0;
  }
  if (pin >= 14) {
    pin -= 14;
    bitWrite(DDRC, pin, val);
  } else if (pin >= 8) {
    pin -= 8;
    bitWrite(DDRB, pin, val);
  } else {
    bitWrite(DDRD, pin, val);
  }
}
