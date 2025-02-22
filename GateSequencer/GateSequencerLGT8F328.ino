//#define USE_LGT_EEPROM_API
//#include <EEPROM.h>

#include <FastLED.h>

#define NUM_LEDS 12

#define DATA_PIN 13

// Define the array of leds
CRGB leds[NUM_LEDS];

long channelColours[8] =      { CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::Yellow, CRGB::Cyan, CRGB::Magenta, CRGB::SeaGreen, CRGB::Orange };
long channelColoursLight[8] = { CRGB::HotPink, CRGB::LightGreen, CRGB::LightBlue, CRGB::LightYellow, CRGB::LightCyan, CRGB::Lavender, CRGB::LimeGreen, CRGB::Coral };
long channelColoursDark[8] =  { CRGB::DarkRed, CRGB::DarkGreen, CRGB::DarkBlue, CRGB::DarkGoldenrod, CRGB::DarkCyan, CRGB::DarkMagenta, CRGB::DarkSeaGreen, CRGB::DarkOrange };

byte switchCols[] = { 6, 7, 8, 9 }; // { 9, 8, 7, 6 };
byte switchRows[] = { 10, 11, 12 }; // { 12, 11, 10 };
bool prevSwitchStates[12];
byte switchMatrix[] =  { 0,	4,	8,	1,	5,	9,	2,	6,	10,	3,	7,	11 };
byte ledMatrix[] = { 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
unsigned long switchLastPressed = 0;
int switchDebounce = 250;

byte readCol = 0;
byte readRow = 0;

byte outputs[] = { 4, 5, A0, A1, A2, A3, A4, A5 };

byte clockPin = 2;
byte resetPin = 3;
byte stopPin = A7;
byte startPin = A6;
bool prevStopState = false;
bool prevStartState = false;

int beats[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
int maxBeats[] = { 8, 8, 8, 8, 8, 8, 8, 8 };
byte patterns[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
int editingChannel = 0;

bool running = true;

byte chooseChannelSwitch = 0;
byte view = 0; // 0 = editing pattern, 1 = choosing channel
byte chooseLengthSwitch = 1;
bool choosingLength = false;
byte randomiseSwitch = 2;
byte fillSwitch = 3;
bool doingFill[] = { false, false, false, false, false, false, false, false };

// Just for debugging
unsigned long lastPrintedSwitchStates = 0;
int printSwitchStatesEvery = 500;
byte ledTestColours[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
bool ledState = false;

void setup() {

  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(63);
  for(int i = 0; i < 12; i++){
    leds[i] = CRGB::Black;
  }
  FastLED.show();

  for(byte i = 0; i < 3; i++){
    pinMode(switchRows[i], INPUT_PULLUP);
  }

  for(byte i = 0; i < 8; i++){
    pinMode(outputs[i], OUTPUT);
  }

  pinMode(clockPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(clockPin), clock, CHANGE);
  pinMode(resetPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(resetPin), reset, RISING);
  pinMode(stopPin, INPUT);
  pinMode(startPin, INPUT);

  //for(byte i = 0; i < 8; i++){
  //  patterns[i] = EEPROM.read(i);
  //}

  loadPatternToLeds();

  // Seems to stop it crashing??
  Serial.begin(250000);

}

void clock(){

  if(!running){
    return;
  }

  if(digitalRead(clockPin)){

    for(int i = 0; i < 8; i++){
      beats[i]++;
      if(beats[i] >= maxBeats[i]){
        beats[i] = 0;
      }
      if(doingFill[i]){
        digitalWrite(outputs[i], HIGH);
      }else{
        digitalWrite(outputs[i], bitRead(patterns[i], beats[i]));
      }
    }

    if(view == 0){
      loadPatternToLeds();
    }

  }else{

    for(int i = 0; i < 8; i++){
      digitalWrite(outputs[i], LOW);
    }

  }

}

void reset(){
  for(int i = 0; i < 8; i++){
    beats[i] = 0;
  }
}

void start(){
  running = true;
}

void stop(){
  running = false;
}

void loop() {

  bool startState = digitalRead(startPin);
  if(startState && !prevStartState){
    start();
  }
  prevStartState = startState;

  bool stopState = digitalRead(stopPin);
  if(stopState && !prevStopState){
    stop();
  }
  prevStopState = stopState;

  byte switchNumber = switchMatrix[(readCol * 3) + readRow];
  bool switchState = !digitalRead(switchRows[readRow]);
  if(switchState && !prevSwitchStates[switchNumber] && millis() - switchLastPressed > switchDebounce){
    if(switchNumber == chooseChannelSwitch){
      view = (view == 1) ? 0 : 1;
      if(view == 1){
        setLedsToChannelColours();
      }
    }else if(switchNumber == chooseLengthSwitch){
      choosingLength = !choosingLength;
      view = 0;
      leds[ledMatrix[chooseLengthSwitch]] = choosingLength ? CRGB::White : CRGB::Black;
      loadPatternToLeds();
    }else if(switchNumber == randomiseSwitch){
      view = 0;
      patterns[editingChannel] = random(256);
      loadPatternToLeds();
    }else if(switchNumber == fillSwitch){
      view = 0;
      doingFill[editingChannel] = !doingFill[editingChannel];
      loadPatternToLeds();
    }else if(switchNumber >= 4){
      byte choiceNumber = switchNumber - 4;
      if(view == 1){
        editingChannel = choiceNumber;
        view = 0;
        loadPatternToLeds();
      }else if(view == 0){
        if(choosingLength){
          maxBeats[editingChannel] = choiceNumber + 1;
        }else{
          bitWrite(patterns[editingChannel], choiceNumber, !bitRead(patterns[editingChannel], choiceNumber));
        }
        loadPatternToLeds();
        //EEPROM.write(editingChannel, patterns[editingChannel]);
      }
    }

    switchLastPressed = millis();
  }
  prevSwitchStates[switchNumber] = switchState;
  readRow++;
  if(readRow >= 3){
    readRow = 0;
    pinMode(switchCols[readCol], INPUT);
    readCol++;
    if(readCol >= 4){
      readCol = 0;
    }
    pinMode(switchCols[readCol], OUTPUT);
    digitalWrite(switchCols[readCol], LOW);
  }

}

void loadPatternToLeds(){

  leds[ledMatrix[chooseChannelSwitch]] = channelColours[editingChannel];
  leds[ledMatrix[fillSwitch]] = doingFill[editingChannel] ? channelColours[editingChannel] : CRGB::Black;

  for(int i = 0; i < 8; i++){
    if(bitRead(patterns[editingChannel], i)){
      leds[ledMatrix[i + 4]] = (beats[editingChannel] == i) ? channelColoursLight[editingChannel] : channelColours[editingChannel];
    }else{
      leds[ledMatrix[i + 4]] = (beats[editingChannel] == i) ? channelColoursDark[editingChannel] : CRGB::Black;
    }
  }

  FastLED.show();

}

void setLedsToChannelColours(){

  for(int i = 0; i < 8; i++){
    leds[ledMatrix[i + 4]] = channelColours[i];
  }

  FastLED.show();

}
