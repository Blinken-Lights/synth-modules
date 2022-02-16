#include  <SPI.h>

//bool debug = true;
bool testDac = false;

byte scales[8][16] = {
        { 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15 }, // chromatic
        
        //0,  2,  2,  1,  2,  2,  2,| 0,  2,  2,  1,  2,  2,  2,| 0,  2
        { 0,  2,  4,  5,  7,  9, 11, 12, 14, 16, 17, 19, 21, 23, 24, 26 }, // major
        
        //0,  2,  1,  2,  2,  1,  2,| 0,  2,  1,  2,  2,  1,  2,| 0,  2
        { 0,  2,  3,  5,  7,  8, 10, 12, 14, 15, 17, 19, 20, 22, 24, 26 }, // minor

        //0,  2,  1,  2,  2,  1,  3,| 0,  2,  1,  2,  2,  1,  3,| 0,  2
        { 0,  2,  3,  5,  7,  8, 11, 12, 14, 15, 17, 19, 20, 23, 24, 26 },// harmonic minor

        //0,  2,  1,  3,  1,  1,  3,| 0,  2,  1,  3,  1,  1,  3,| 0,  2
        { 0,  2,  3,  6,  7,  8, 11, 12, 14, 15, 18, 19, 20, 23, 24, 26 },// 'hungarian' minor

        //0,  2,  1,  2,  2,  2,  1,| 0,  2,  1,  2,  2,  2,  1,| 0,  2
        { 0,  2,  3,  5,  7,  9, 10, 12, 14, 15, 17, 19, 21, 22, 24, 26 },// dorian
        
        //0,  1,  2,  2,  2,  1,  2,| 0,  1,  2,  2,  2,  1,  2,| 0,  1
        { 0,  1,  3,  5,  7,  8, 10, 12, 13, 15, 17, 19, 20, 22, 24, 25 },// phrygian

        //0,  1,  2,  1,  2,  2,  2,| 0,  1,  2,  1,  2,  2,  2,| 0,  1
        { 0,  1,  3,  4,  6,  8, 10, 12, 13, 15, 16, 18, 20, 22, 24, 25  },// super locrian
/*
        //0,  2,  1,  4,  1,| 0,  2,  1,  4,  1,| 0,  2,  1,  4,  1,| 0
        { 0,  2,  3,  7,  8, 12, 14, 15, 19, 20, 24, 26, 27, 31, 32, 36 },// hirajoshi

        //0,  2,  2,  3,  2,| 0,  2,  2,  3,  2,| 0,  2,  2,  3,  2,| 0
        { 0,  2,  4,  7,  9, 12, 14, 16, 19, 21, 24, 26, 28, 31, 33, 36 },// chinese */

        };

byte maxNotes = 64;
// bitshift note value << 5
// so max DAC output voltage at internal gain x2 will be 2048
// gain of op-amp should be (5333.3333333 / 2048) = 2.6
// since this is a non-inverting configuration, the scale resistor should be (2.6 - 1.0) x 100k = 160k

byte scalePin = A5;
byte transposePin = A4;
byte transposeCVPin = A7;
byte rotateCVPin = A6;

byte gateInPins[] = { 7, 4, 6, 5, 3, 2 }; // pins on Arduino
byte octavePins[] = { 0, 1 }; // addresses within gateInPins
byte notePins[] = { 2, 4, 3, 5 }; // addresses within gateInPins

byte newNotePin = 8;
byte trigGatePin = 9;
byte trigGateSwitchPin = A3;

int trigOutLength = 60; // milliseconds
bool trigOutHigh = false;
unsigned long trigOutStarted = 0;

bool prevTrigState = false;
byte octave = 0;
byte note = 0;
byte prevOctave = 0;
byte prevNote = 0;

byte scale = 0;
byte transpose = 0;
byte rotate = 0;

int tolerance = 10;
int prevRawScale = 0;
int prevRawTranspose = 0;
int prevRawRotate = 0;

// only used for debugging
byte prevScale = 0;
byte prevTranspose = 0;
byte prevRotate = 0;
byte prevIsTrig = 0;
bool prevOctPinStates[] = { false, false };
bool prevNotePinStates[] = { false, false, false, false, false, false };

void setup() {

  Serial.begin(250000);

  SPI.begin();  
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0);
  pinMode(10, OUTPUT); // for DAC CS pin

  pinMode(trigGatePin, INPUT); // normalled to 5V via 2k resistor !
  pinMode(trigGateSwitchPin, INPUT_PULLUP);
  
  // analogue inputs
  pinMode(scalePin, INPUT);
  pinMode(transposePin, INPUT);
  pinMode(transposeCVPin, INPUT);
  pinMode(rotateCVPin, INPUT);

  pinMode(newNotePin, OUTPUT); // no 100k pull down resistor on this one (because it's an output) !

  for(int i = 0; i < 8; i++){
    pinMode(gateInPins[i], INPUT);
  }

  if(testDac){
    for(int j = 0; j < 1000; j++){
      for(int i = 0; i <= 2048; i++){
        WriteRegister(i, 0);
      }
    }
    for(int j = 0; j < 5; j++){
      for(int i = 0; i <= 5; i++){
        Serial.println(i);
        WriteRegister((i * 12) << 5, 0);
        delay(2500);
      }
    }
    WriteRegister(maxNotes << 5, 0); // should measure 5.333V
  }
  WriteRegister(0, 0);

}

void loop() {

  bool isTrig = opt_read(trigGateSwitchPin);
  bool trigState = opt_read(trigGatePin);

  // if trigger rises, or gate mode is selected..
  if( (isTrig && trigState && !prevTrigState) || (!isTrig && trigState)){
    // ..check for a note change

    byte rotatedOctavePins[] = { octavePins[0], octavePins[1] };
    byte rotatedNotePins[] = { notePins[0], notePins[1], notePins[2], notePins[3] };

    for(int i = 0; i < 2; i++){
      rotatedOctavePins[i] = rotatedOctavePins[i] + rotate;
      if(rotatedOctavePins[i] >= 6){
        rotatedOctavePins[i]-= 6;
      }
    }
    for(int i = 0; i < 4; i++){
      rotatedNotePins[i] = rotatedNotePins[i] + rotate;
      if(rotatedNotePins[i] >= 6){
        rotatedNotePins[i]-= 6;
      }
    }
    
    byte currentOctave = opt_read(gateInPins[rotatedOctavePins[0]]) | (opt_read(gateInPins[rotatedOctavePins[1]]) << 1);
    byte currentNote = opt_read(gateInPins[rotatedNotePins[0]]) | (opt_read(gateInPins[rotatedNotePins[1]]) << 1) | (opt_read(gateInPins[rotatedNotePins[2]]) << 2) | (opt_read(gateInPins[rotatedNotePins[3]]) << 3);

    if(currentOctave != octave || currentNote != note){
      // note change detected

      int transposedNote = currentNote + transpose;
      if(transposedNote > 15){
        transposedNote-= 15;
      }
      int noteNumber = (currentOctave * 12) + (scales[scale][transposedNote]);
      if(noteNumber > maxNotes){
        noteNumber-= maxNotes;
      }
      int dacValue = noteNumber << 5;
      WriteRegister(dacValue, 0);

      /*if(debug){
        Serial.print("oct: ");
        Serial.print(currentOctave);
        Serial.print("note: ");
        Serial.print(currentNote);
        Serial.print("noteNumber: ");
        Serial.print(noteNumber);
        Serial.print("dac value: ");
        Serial.println(dacValue);
      }*/

      trigOutHigh = true;
      trigOutStarted = millis();
      opt_write(newNotePin, HIGH);

      octave = currentOctave;
      note = currentNote;
    }
  }

  if((millis() - trigOutStarted) > trigOutLength){
    opt_write(newNotePin, LOW);
  }

  int rawScale = analogRead(scalePin);
  int rawTranspose = analogRead(transposePin) + analogRead(transposeCVPin);
  int rawRotate = analogRead(rotateCVPin);

  if(abs(rawScale - prevRawScale) > tolerance){
    scale = rawScale >> 7; // 0 - 7
    prevRawScale = rawScale;
  }
  if(abs(rawTranspose - prevRawTranspose) > tolerance){
    transpose = rawTranspose >> 6; // 0 - 31
    if(transpose >= 16){
      transpose-= 16; // so if the transpose knob is at maximum, the transpose CV will wrap around to 0 - 15
    }
    prevRawTranspose = rawTranspose;
  }
  if(abs(rawRotate - prevRawRotate) > tolerance){
    rotate = rawRotate >> 7; // 0 - 7
    prevRawRotate = rawRotate;
  }

  /*if(debug){
    if(prevScale != scale){
      Serial.print("scale: ");
      Serial.println(scale);
      prevScale = scale;
    }
    if(prevTranspose != transpose){
      Serial.print("transpose: ");
      Serial.println(transpose);
      prevTranspose = transpose;
    }
    if(prevRotate != rotate){
      Serial.print("rotate: ");
      Serial.println(rotate);
      prevRotate = rotate;
    }
    if(prevTrigState != trigState){
      Serial.print("trigState: ");
      Serial.println(trigState?"HIGH":"LOW");
    }
    if(prevIsTrig != isTrig){
      Serial.print("isTrig: ");
      Serial.println(isTrig?"Triggered":"Gated");
      prevIsTrig = isTrig;
    }
    for(int i = 0; i < 2; i++){
      bool p = opt_read(gateInPins[octavePins[i]]);
      if(p != prevOctPinStates[i]){
        Serial.print("oct ");
        Serial.print(i);
        Serial.print(": ");
        Serial.println(p?"HIGH":"LOW");
      }
      prevOctPinStates[i] = p;
    }
    for(int i = 0; i < 4; i++){
      bool p = opt_read(gateInPins[notePins[i]]);
      if(p != prevNotePinStates[i]){
        Serial.print("note ");
        Serial.print(i);
        Serial.print(": ");
        Serial.println(p?"HIGH":"LOW");
      }
      prevNotePinStates[i] = p;
    }
  }*/

  prevTrigState = trigState;
}

void WriteRegister(unsigned int val, byte dac_channel){
  byte lowByte = val & 0xff;
  byte highByte = ((val >> 8) & 0xff) | (dac_channel << 7) | B00010000;

  bitClear(PORTB, 2);
  SPI.transfer(highByte);
  SPI.transfer(lowByte);
  bitSet(PORTB, 2);
}

bool opt_read(byte pin){
  if(pin >= 14){
    pin-=14;
    return bitRead(PINC, pin);
  }else if(pin >= 8){
    pin-=8;
    return bitRead(PINB, pin);
  }else{
    return bitRead(PIND, pin);
  }
}

void opt_write(byte pin, bool val){
  if(pin >= 14){
    pin-=14;
    bitWrite(PORTC, pin, val);
  }else if(pin >= 8){
    pin-=8;
    bitWrite(PORTB, pin, val);
  }else{
    bitWrite(PORTD, pin, val);
  }
}

void opt_mode(byte pin, byte val){
  if(val == 2){
    opt_write(pin, HIGH);
    val = 0;
  }
  if(pin >= 14){
    pin-=14;
    bitWrite(DDRC, pin, val);
  }else if(pin >= 8){
    pin-=8;
    bitWrite(DDRB, pin, val);
  }else{
    bitWrite(DDRD, pin, val);
  }
}
