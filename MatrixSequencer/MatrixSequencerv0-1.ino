#include <MD_MAX72xx.h>
#include <MIDI.h>
#include <EEPROM.h>

// Define the number of devices we have in the chain and the hardware interface
// NOTE: These pin numbers will probably not work with your hardware and may
// need to be adapted
#define HARDWARE_TYPE MD_MAX72XX::DR1CR1RR0_HW // ICSTATION_HW //DR1CR1RR0_HW  //ICSTATION_HW
#define MAX_DEVICES 4
// Text parameters
#define CHAR_SPACING  1 // pixels between characters

#define CLK_PIN 13   // or SCK
#define DATA_PIN 11  // or MOSI
#define CS_PIN 10    // or SS

// SPI hardware interface
MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

MIDI_CREATE_DEFAULT_INSTANCE();

byte midiChannel = 1;  // TO DO: set output channel via knob
byte midiNoteOn = 255;
// access with pgm_read_byte( &(scaleNotes[scaleNumber][note]) );
const uint8_t scaleNotes[16][128] PROGMEM = {
  // chromatic
  { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127 },
  // dorian
  { 0, 1, 3, 5, 7, 8, 10, 12, 13, 15, 17, 19, 20, 22, 24, 25, 27, 29, 31, 32, 34, 36, 37, 39, 41, 43, 44, 46, 48, 49, 51, 53, 55, 56, 58, 60, 61, 63, 65, 67, 68, 70, 72, 73, 75, 77, 79, 80, 82, 84, 85, 87, 89, 91, 92, 94, 96, 97, 99, 101, 103, 104, 106, 108, 109, 111, 113, 115, 116, 118, 120, 121, 123, 125, 127, 104, 106, 108, 109, 111, 113, 115, 116, 118, 120, 121, 123, 125, 127, 104, 106, 108, 109, 111, 113, 115, 116, 118, 120, 121, 123, 125, 127, 104, 106, 108, 109, 111, 113, 115, 116, 118, 120, 121, 123, 125, 127, 104, 106, 108, 109, 111, 113, 115, 116, 118, 120, 121 },
  // phrygian
  { 0, 2, 4, 6, 7, 9, 11, 12, 14, 16, 18, 19, 21, 23, 24, 26, 28, 30, 31, 33, 35, 36, 38, 40, 42, 43, 45, 47, 48, 50, 52, 54, 55, 57, 59, 60, 62, 64, 66, 67, 69, 71, 72, 74, 76, 78, 79, 81, 83, 84, 86, 88, 90, 91, 93, 95, 96, 98, 100, 102, 103, 105, 107, 108, 110, 112, 114, 115, 117, 119, 120, 122, 124, 126, 127, 105, 107, 108, 110, 112, 114, 115, 117, 119, 120, 122, 124, 126, 127, 105, 107, 108, 110, 112, 114, 115, 117, 119, 120, 122, 124, 126, 127, 105, 107, 108, 110, 112, 114, 115, 117, 119, 120, 122, 124, 126, 127, 105, 107, 108, 110, 112, 114, 115, 117, 119, 120, 122 },
  // lydian
  { 0, 2, 4, 5, 7, 9, 10, 12, 14, 16, 17, 19, 21, 22, 24, 26, 28, 29, 31, 33, 34, 36, 38, 40, 41, 43, 45, 46, 48, 50, 52, 53, 55, 57, 58, 60, 62, 64, 65, 67, 69, 70, 72, 74, 76, 77, 79, 81, 82, 84, 86, 88, 89, 91, 93, 94, 96, 98, 100, 101, 103, 105, 106, 108, 110, 112, 113, 115, 117, 118, 120, 122, 124, 125, 127, 105, 106, 108, 110, 112, 113, 115, 117, 118, 120, 122, 124, 125, 127, 105, 106, 108, 110, 112, 113, 115, 117, 118, 120, 122, 124, 125, 127, 105, 106, 108, 110, 112, 113, 115, 117, 118, 120, 122, 124, 125, 127, 105, 106, 108, 110, 112, 113, 115, 117, 118, 120, 122 },
  // mixolydian
  { 0, 2, 3, 5, 7, 8, 10, 12, 14, 15, 17, 19, 20, 22, 24, 26, 27, 29, 31, 32, 34, 36, 38, 39, 41, 43, 44, 46, 48, 50, 51, 53, 55, 56, 58, 60, 62, 63, 65, 67, 68, 70, 72, 74, 75, 77, 79, 80, 82, 84, 86, 87, 89, 91, 92, 94, 96, 98, 99, 101, 103, 104, 106, 108, 110, 111, 113, 115, 116, 118, 120, 122, 123, 125, 127, 104, 106, 108, 110, 111, 113, 115, 116, 118, 120, 122, 123, 125, 127, 104, 106, 108, 110, 111, 113, 115, 116, 118, 120, 122, 123, 125, 127, 104, 106, 108, 110, 111, 113, 115, 116, 118, 120, 122, 123, 125, 127, 104, 106, 108, 110, 111, 113, 115, 116, 118, 120, 122 },
  // aeolian
  { 0, 1, 3, 5, 6, 8, 10, 12, 13, 15, 17, 18, 20, 22, 24, 25, 27, 29, 30, 32, 34, 36, 37, 39, 41, 42, 44, 46, 48, 49, 51, 53, 54, 56, 58, 60, 61, 63, 65, 66, 68, 70, 72, 73, 75, 77, 78, 80, 82, 84, 85, 87, 89, 90, 92, 94, 96, 97, 99, 101, 102, 104, 106, 108, 109, 111, 113, 114, 116, 118, 120, 121, 123, 125, 126, 104, 106, 108, 109, 111, 113, 114, 116, 118, 120, 121, 123, 125, 126, 104, 106, 108, 109, 111, 113, 114, 116, 118, 120, 121, 123, 125, 126, 104, 106, 108, 109, 111, 113, 114, 116, 118, 120, 121, 123, 125, 126, 104, 106, 108, 109, 111, 113, 114, 116, 118, 120, 121 },
  // locrian
  { 0, 2, 4, 5, 7, 9, 11, 12, 14, 16, 17, 19, 21, 23, 24, 26, 28, 29, 31, 33, 35, 36, 38, 40, 41, 43, 45, 47, 48, 50, 52, 53, 55, 57, 59, 60, 62, 64, 65, 67, 69, 71, 72, 74, 76, 77, 79, 81, 83, 84, 86, 88, 89, 91, 93, 95, 96, 98, 100, 101, 103, 105, 107, 108, 110, 112, 113, 115, 117, 119, 120, 122, 124, 125, 127, 105, 107, 108, 110, 112, 113, 115, 117, 119, 120, 122, 124, 125, 127, 105, 107, 108, 110, 112, 113, 115, 117, 119, 120, 122, 124, 125, 127, 105, 107, 108, 110, 112, 113, 115, 117, 119, 120, 122, 124, 125, 127, 105, 107, 108, 110, 112, 113, 115, 117, 119, 120, 122 },
  // lydian_domiant
  { 0, 2, 4, 5, 7, 8, 10, 12, 14, 16, 17, 19, 20, 22, 24, 26, 28, 29, 31, 32, 34, 36, 38, 40, 41, 43, 44, 46, 48, 50, 52, 53, 55, 56, 58, 60, 62, 64, 65, 67, 68, 70, 72, 74, 76, 77, 79, 80, 82, 84, 86, 88, 89, 91, 92, 94, 96, 98, 100, 101, 103, 104, 106, 108, 110, 112, 113, 115, 116, 118, 120, 122, 124, 125, 127, 104, 106, 108, 110, 112, 113, 115, 116, 118, 120, 122, 124, 125, 127, 104, 106, 108, 110, 112, 113, 115, 116, 118, 120, 122, 124, 125, 127, 104, 106, 108, 110, 112, 113, 115, 116, 118, 120, 122, 124, 125, 127, 104, 106, 108, 110, 112, 113, 115, 116, 118, 120, 122 },
  // super_locrian
  { 0, 2, 3, 5, 7, 9, 11, 12, 14, 15, 17, 19, 21, 23, 24, 26, 27, 29, 31, 33, 35, 36, 38, 39, 41, 43, 45, 47, 48, 50, 51, 53, 55, 57, 59, 60, 62, 63, 65, 67, 69, 71, 72, 74, 75, 77, 79, 81, 83, 84, 86, 87, 89, 91, 93, 95, 96, 98, 99, 101, 103, 105, 107, 108, 110, 111, 113, 115, 117, 119, 120, 122, 123, 125, 127, 105, 107, 108, 110, 111, 113, 115, 117, 119, 120, 122, 123, 125, 127, 105, 107, 108, 110, 111, 113, 115, 117, 119, 120, 122, 123, 125, 127, 105, 107, 108, 110, 111, 113, 115, 117, 119, 120, 122, 123, 125, 127, 105, 107, 108, 110, 111, 113, 115, 117, 119, 120, 122 },
  // hirajoshi
  { 0, 1, 5, 6, 10, 12, 13, 17, 18, 22, 24, 25, 29, 30, 34, 36, 37, 41, 42, 46, 48, 49, 53, 54, 58, 60, 61, 65, 66, 70, 72, 73, 77, 78, 82, 84, 85, 89, 90, 94, 96, 97, 101, 102, 106, 108, 109, 113, 114, 118, 120, 121, 125, 126, 106, 108, 109, 113, 114, 118, 120, 121, 125, 126, 106, 108, 109, 113, 114, 118, 120, 121, 125, 126, 106, 108, 109, 113, 114, 118, 120, 121, 125, 126, 106, 108, 109, 113, 114, 118, 120, 121, 125, 126, 106, 108, 109, 113, 114, 118, 120, 121, 125, 126, 106, 108, 109, 113, 114, 118, 120, 121, 125, 126, 106, 108, 109, 113, 114, 118, 120, 121, 125, 126, 106, 108, 109, 113 },
  // minor_pentatonic
  { 0, 2, 4, 7, 9, 12, 14, 16, 19, 21, 24, 26, 28, 31, 33, 36, 38, 40, 43, 45, 48, 50, 52, 55, 57, 60, 62, 64, 67, 69, 72, 74, 76, 79, 81, 84, 86, 88, 91, 93, 96, 98, 100, 103, 105, 108, 110, 112, 115, 117, 120, 122, 124, 127, 105, 108, 110, 112, 115, 117, 120, 122, 124, 127, 105, 108, 110, 112, 115, 117, 120, 122, 124, 127, 105, 108, 110, 112, 115, 117, 120, 122, 124, 127, 105, 108, 110, 112, 115, 117, 120, 122, 124, 127, 105, 108, 110, 112, 115, 117, 120, 122, 124, 127, 105, 108, 110, 112, 115, 117, 120, 122, 124, 127, 105, 108, 110, 112, 115, 117, 120, 122, 124, 127, 105, 108, 110, 112 },
  // major_pentatonic
  { 0, 2, 5, 7, 10, 12, 14, 17, 19, 22, 24, 26, 29, 31, 34, 36, 38, 41, 43, 46, 48, 50, 53, 55, 58, 60, 62, 65, 67, 70, 72, 74, 77, 79, 82, 84, 86, 89, 91, 94, 96, 98, 101, 103, 106, 108, 110, 113, 115, 118, 120, 122, 125, 127, 106, 108, 110, 113, 115, 118, 120, 122, 125, 127, 106, 108, 110, 113, 115, 118, 120, 122, 125, 127, 106, 108, 110, 113, 115, 118, 120, 122, 125, 127, 106, 108, 110, 113, 115, 118, 120, 122, 125, 127, 106, 108, 110, 113, 115, 118, 120, 122, 125, 127, 106, 108, 110, 113, 115, 118, 120, 122, 125, 127, 106, 108, 110, 113, 115, 118, 120, 122, 125, 127, 106, 108, 110, 113 },
  // minor_blues
  { 0, 2, 3, 4, 7, 9, 12, 14, 15, 16, 19, 21, 24, 26, 27, 28, 31, 33, 36, 38, 39, 40, 43, 45, 48, 50, 51, 52, 55, 57, 60, 62, 63, 64, 67, 69, 72, 74, 75, 76, 79, 81, 84, 86, 87, 88, 91, 93, 96, 98, 99, 100, 103, 105, 108, 110, 111, 112, 115, 117, 120, 122, 123, 124, 127, 105, 108, 110, 111, 112, 115, 117, 120, 122, 123, 124, 127, 105, 108, 110, 111, 112, 115, 117, 120, 122, 123, 124, 127, 105, 108, 110, 111, 112, 115, 117, 120, 122, 123, 124, 127, 105, 108, 110, 111, 112, 115, 117, 120, 122, 123, 124, 127, 105, 108, 110, 111, 112, 115, 117, 120, 122, 123, 124, 127, 105, 108, 110 },
  // major_blues
  { 0, 1, 2, 5, 7, 10, 12, 13, 14, 17, 19, 22, 24, 25, 26, 29, 31, 34, 36, 37, 38, 41, 43, 46, 48, 49, 50, 53, 55, 58, 60, 61, 62, 65, 67, 70, 72, 73, 74, 77, 79, 82, 84, 85, 86, 89, 91, 94, 96, 97, 98, 101, 103, 106, 108, 109, 110, 113, 115, 118, 120, 121, 122, 125, 127, 106, 108, 109, 110, 113, 115, 118, 120, 121, 122, 125, 127, 106, 108, 109, 110, 113, 115, 118, 120, 121, 122, 125, 127, 106, 108, 109, 110, 113, 115, 118, 120, 121, 122, 125, 127, 106, 108, 109, 110, 113, 115, 118, 120, 121, 122, 125, 127, 106, 108, 109, 110, 113, 115, 118, 120, 121, 122, 125, 127, 106, 108, 109 },
  // whole_half_diminished
  { 0, 1, 3, 4, 6, 7, 9, 10, 12, 13, 15, 16, 18, 19, 21, 22, 24, 25, 27, 28, 30, 31, 33, 34, 36, 37, 39, 40, 42, 43, 45, 46, 48, 49, 51, 52, 54, 55, 57, 58, 60, 61, 63, 64, 66, 67, 69, 70, 72, 73, 75, 76, 78, 79, 81, 82, 84, 85, 87, 88, 90, 91, 93, 94, 96, 97, 99, 100, 102, 103, 105, 106, 108, 109, 111, 112, 114, 115, 117, 118, 120, 121, 123, 124, 126, 127, 105, 106, 108, 109, 111, 112, 114, 115, 117, 118, 120, 121, 123, 124, 126, 127, 105, 106, 108, 109, 111, 112, 114, 115, 117, 118, 120, 121, 123, 124, 126, 127, 105, 106, 108, 109, 111, 112, 114, 115, 117, 118 },
  // half_whole_diminished
  { 0, 2, 3, 5, 6, 8, 9, 11, 12, 14, 15, 17, 18, 20, 21, 23, 24, 26, 27, 29, 30, 32, 33, 35, 36, 38, 39, 41, 42, 44, 45, 47, 48, 50, 51, 53, 54, 56, 57, 59, 60, 62, 63, 65, 66, 68, 69, 71, 72, 74, 75, 77, 78, 80, 81, 83, 84, 86, 87, 89, 90, 92, 93, 95, 96, 98, 99, 101, 102, 104, 105, 107, 108, 110, 111, 113, 114, 116, 117, 119, 120, 122, 123, 125, 126, 104, 105, 107, 108, 110, 111, 113, 114, 116, 117, 119, 120, 122, 123, 125, 126, 104, 105, 107, 108, 110, 111, 113, 114, 116, 117, 119, 120, 122, 123, 125, 126, 104, 105, 107, 108, 110, 111, 113, 114, 116, 117, 119 },
};

const char scaleNames[16][9] = {
  "Chroma  \0",  // chromatic
  "Dorian  \0",  // dorian
  "Phrygia \0",  // phrygian
  "Lydian  \0",  // lydian
  "Mixolidi\0",  // mixolydian
  "Aeolian \0",  // aeolian
  "Locrian \0",  // locrian
  "LyDomin \0",  // lydian_domiant
  "SupLocr \0",  // super_locrian
  "Hirajos \0",  // hirajoshi
  "MinPenta\0",  // minor_pentatonic
  "MajPenta\0",  // major_pentatonic
  "MinBlues\0",  // minor_blues
  "MaxBlues\0",  // major_blues
  "WHDimini\0",  // whole_half_diminished
  "HWDimini\0"   // half_whole_diminished
};
unsigned long lastShowedText = 0;
int showTextFor = 3000;
char digitBuffer[3];

bool debug = false;

byte row = 0;
byte prevRow = 0;
unsigned long lastChangedRow;
unsigned long lastChangedLength;
unsigned long lastChangedPage;
int showChangesFor = 750;

byte page = 0;
byte loadedPage = 0;
bool isFirstLoad = true;
byte prevPage = 255;  // to trigger an initial refresh

byte p = 0;
int analoguePins[] = { A0, A1, A2, A3, A4, A5, A6, A7 };
int prevVals[] = { -1, -1, -1, -1, -1, -1, -1, -1 };  // to trigger an initial refresh

byte outputPins[] = { 3, 4, 5, 6 };

byte clearPin = 7;
byte randomisePin = 8;
byte stopStartResetPin = 9;

byte clockInPin = 2;
// TO DO: Master clock module with 8 pots and 8 outputs, for different divisions / multiplications of the clock speed (accuracy of 32)
// Put this in the centre of the rack to reduce the cable lengths needed.
// Clock speed should be changed by an encoder so there will be no jitter
bool prevClockInValue = false;
bool externalClock = true;

byte applyButton = 12;
bool prevApplyVal = false;
// TO DO: pins 7, 8, 9 can have other switches.
// For instance, to randomise the row or page
bool wrapLength = false; // <-- and/or turn this on/off?

byte zeroThreshold = 10;
byte jitterThreshold = 0;  // hopefully a filter on the pot power will solve this

int prevLength = 0;
int prevEvents = 0;
int prevOffset = 0;

bool newSequenceBeforeWrapping[32];
bool newSequence[32];

volatile byte grid[8][4];

int patternValues[8][3];
byte previewPattern[3];
byte previewGrid[4];

byte mutationRate[8];
int barsSinceLastMutation[8];
byte prevMutateValue = 0;

byte param = 0;
byte prevParam = 0;
byte option = 0;
byte prevOpt = 0;

byte maxPatternLength = 32;  //64;
byte maxPatternBytes;
byte potAccuracyAdjustment = 5;  //4;
byte patternLength;
byte nextBarPatternLength;
byte currentChosenPatternLength;
bool lengthFlashState = false;
int flashLength = 250;
unsigned long lastFlashedLength;
bool firstLoadLength = true;

byte patternStart = 0;
int speed = 45;  //10;//25;//0;
int displayDebounce = 25;
unsigned long lastWroteDisplay = 0;
//byte writeDisplayRow = 0;
byte writeDisplayCol = 0;
int minimumSpeed = 80;
byte scale = 5;
byte prevScale = 5;

unsigned long lastBeatTime = 0;

volatile byte beat = 0;
volatile bool isHalfBeat = false;
volatile bool isRunning = false;

byte beatChanged = false;
byte patternChanged = false;

bool lastClearState = false;
bool lastRandomiseState = false;
bool lastStopStartResetState = false;
unsigned long buttonLastPressed = 0;
int buttonDebounce = 250;

void setup() {

  if(debug){
    Serial.begin(250000);
  }else{
    MIDI.begin(MIDI_CHANNEL_OMNI);
    MIDI.turnThruOff();
  }

  mx.begin();

  mx.control(MD_MAX72XX::INTENSITY, 0);

  for (byte i = 0; i < 4; i++) {
    pinMode(outputPins[i], OUTPUT);
  }

  pinMode(applyButton, INPUT_PULLUP);
  pinMode(clearPin, INPUT_PULLUP);
  pinMode(randomisePin, INPUT_PULLUP);
  pinMode(stopStartResetPin, INPUT_PULLUP);

  pinMode(clockInPin, INPUT);  // TO DO: needs pull down resistor

  attachInterrupt(digitalPinToInterrupt(clockInPin), advanceBeat, RISING);

  patternLength = maxPatternLength;
  maxPatternBytes = maxPatternLength / 8;
  for (byte r = 0; r < 8; r++) {
    patternValues[r][0] = maxPatternLength;
  }

  // to test mutations
  for (byte r = 0; r < 8; r++) {
    //mutationRate[r] = 255 - random(0, 31);
  }

  /*for(byte i = 0; i < 16; i++){
    scaleNames[i][5] = '\0';
  }*/

  lastShowedText = millis() + 3000;

  debugPattern();
  refreshDisplay();
  if (debug) {
    //delay(20000);
  }

}

void loop() {

  beatChanged = false;
  patternChanged = false;

  bool applyVal = !opt_read(applyButton);
  if (applyVal && !prevApplyVal) {
    applyPattern();
  }
  prevApplyVal = applyVal;

  if(millis() - buttonLastPressed > buttonDebounce){

    bool clearVal = !opt_read(clearPin);
    bool randomiseVal = !opt_read(randomisePin);
    bool stopStartResetVal = !opt_read(stopStartResetPin);

    if(clearVal && !lastClearState){
      buttonLastPressed = millis();
      clearPattern();
    }else if(randomiseVal && !lastRandomiseState){
      buttonLastPressed = millis();
      randomisePattern();
    }else if(stopStartResetVal && !lastStopStartResetState){
      buttonLastPressed = millis();
      stopStart();
    }

    lastClearState = clearVal;
    lastRandomiseState = randomiseVal;
    lastStopStartResetState = stopStartResetVal;
  }

  int analogueValue = analogRead(analoguePins[p]);
  int diff = abs(prevVals[p] - analogueValue);
  if(diff >= jitterThreshold){
    prevVals[p] = analogueValue;

    // TO DO: p has changed
    // TO DO: horizontal slider for memory slot
    // TO DO: when the horizontal slider changes, loadPageFromEeprom();
    if (p == 0) {  // 8 way switch
      row = 7 - (analogueValue >> 7);
      if (row != prevRow) {
        loadPattern();
        lastChangedRow = millis();
      }
      prevRow = row;
    }else if(p == 1){ // 8 way switch
      page = analogueValue >> 7;
      if(page != prevPage){
        if(applyVal || isFirstLoad){
          loadPageFromEeprom();
          isFirstLoad = false;
        }
        lastChangedPage = millis();
      }
      prevPage = page;
    }else if(p == 7){
      scale = analogueValue >> 6;
      if(scale != prevScale){
        showScaleText();
      }
      prevScale = scale;
    }else{
      if(analogueValue < zeroThreshold){
        analogueValue = 0;
      }else if(p != 2){
        analogueValue = (analogueValue >> potAccuracyAdjustment);
      }else{
        analogueValue = (analogueValue >> potAccuracyAdjustment) + 1;
      }
      if(p == 2){
        if(analogueValue != prevLength){
          previewPattern[0] = analogueValue;
          prevLength = analogueValue;
          patternChanged = true;
        }
      }else if(p == 3){
        if(analogueValue != prevEvents){
          previewPattern[1] = analogueValue;
          prevEvents = analogueValue;
          patternChanged = true;
        }
      }else if(p == 4){
        if(analogueValue != prevOffset){
          previewPattern[2] = 32 - analogueValue;
          prevOffset = analogueValue;
          patternChanged = true;
        }
      }else if(p == 5){
        if(applyVal || firstLoadLength){
          patternLength = analogueValue;
        }
        nextBarPatternLength = analogueValue;
        if(currentChosenPatternLength != analogueValue){
          lastChangedLength = millis();
        }
        currentChosenPatternLength = analogueValue;
        firstLoadLength = false;
      }else if(p == 6){
        if(analogueValue != prevMutateValue){
          for(byte r = 0; r < 8; r++){
            mutationRate[r] = analogueValue;
          }
          prevMutateValue = analogueValue;
        }
      }
    }
  }

  p++;
  if (p >= 8) {
    p = 0;
  }

  if(patternChanged){
    refreshPattern(row);
  }

  refreshDisplay();
}

void checkBeat() {

  unsigned long m = millis();

  if (externalClock) {
    bool clockInValue = opt_read(clockInPin);
    if (!clockInValue || prevClockInValue) {  // detect rising edge
      prevClockInValue = clockInValue;
      return;
    }
    prevClockInValue = clockInValue;
  } else {
    if (speed == 0 || m - lastBeatTime < speed) {
      return;
    }
    lastBeatTime = m;
  }

  advanceBeat();
}

void advanceBeat() {

  if(!isRunning){
    return;
  }

  byte prevBeat = beat;

  //if(!isHalfBeat){
  //  beat++;
  //}

  byte thisGridIndex = beat / 8;// (maxPatternLength - 1 - beat) / 8;
  byte thisGridBit = 7 - (beat % 8);//((maxPatternLength - 1 - beat) % 8);

  bool beatChannel = false;
  int note = 0;
  bool noteFound = false;
  for (byte br = 0; br < 8; br++) {
    if (!isHalfBeat) {
      beatChannel = bitRead(grid[br][thisGridIndex], thisGridBit);

      if (beatChannel && br >= 4) {
        noteFound = true;
        if (br > 4) {
          note += (1 << (br - 4));
        }
      }
    }
    if (br < 4) {
      opt_write(outputPins[br], beatChannel);
    }
  }

  byte scaleNote = pgm_read_byte(&(scaleNotes[scale][note]));

  if (isHalfBeat) {
    if (midiNoteOn < 255) {
      MIDI.sendNoteOff(midiNoteOn, 0, midiChannel);
      midiNoteOn = 255;
    }
  } else if (noteFound) {
    MIDI.sendNoteOn(scaleNote, 127, midiChannel);
    midiNoteOn = scaleNote;
  }

  if (!isHalfBeat) {
    beat++;
    if (beat > patternLength) {
      beat = patternStart;
      patternLength = nextBarPatternLength;
      if(loadedPage != page){
        loadPageFromEeprom();
      }
      for (byte r = 0; r < 8; r++) {
        barsSinceLastMutation[r]++;
        byte chooseRow = round(random(8));
        if (mutationRate[r] > 0 && barsSinceLastMutation[r] >= 32 - mutationRate[r]) {
          barsSinceLastMutation[r] = 0;
          if(r == chooseRow){
            randomiseRow(r);
          }
          //byte changeParameter = random(3);
          /*for(byte changeParameter = 0; changeParameter < 3; changeParameter++){
            byte changeParameterBy = round(random(2)) - 1;
            patternValues[r][changeParameter] += changeParameterBy;
            if (patternValues[r][changeParameter] < 0) {
              patternValues[r][changeParameter] = 0;
            } else if (patternValues[r][changeParameter] > maxPatternLength) {
              patternValues[r][changeParameter] = maxPatternLength;
            }
          }
          if (bresenhamEuclidean(patternValues[r][0], patternValues[r][1], patternValues[r][2])) {

            for (byte i = 0; i < maxPatternLength; i++) {

              byte sequenceIndex = wrapLength ? i % patternValues[r][0] : i;
              byte gridIndex = i / 8;
              byte gridBit = i % 8;

              bitWrite(grid[r][gridIndex], 7 - gridBit, newSequence[sequenceIndex] == 1);
            }
          }*/
        }
      }
    }
    isHalfBeat = true;
    beatChanged = true;
  } else {
    isHalfBeat = false;
  }
}

void refreshPattern(byte r) {

  if (euclideanPattern(previewPattern[0], previewPattern[1], previewPattern[2])) {

    for (byte i = 0; i < maxPatternLength; i++) {

      byte sequenceIndex = wrapLength ? i % previewPattern[0] : i;
      byte gridIndex = i / 8;
      byte gridBit = i % 8;

      bitWrite(previewGrid[gridIndex], 7 - gridBit, newSequence[sequenceIndex] == 1);

      //mx.setPoint(r, 31 - i, newSequence[sequenceIndex] == 1);
    }

    debugPattern();
  }
}

void applyPattern() {
  for (byte i = 0; i < maxPatternBytes; i++) {
    grid[row][i] = previewGrid[i];
  }
  for (byte i = 0; i < 3; i++) {
    patternValues[row][i] = previewPattern[i];
  }
  updateRowToEeprom(row);

  debugPattern();
}

void loadPattern() {

  debugPattern();

  // TO DO: optimise to avoid rewriting whole display
  //refreshDisplay();
  for (byte gridIndex = 0; gridIndex < maxPatternBytes; gridIndex++) {
    for (byte gridBit = 0; gridBit < 8; gridBit++) {
      byte col = ((3 - gridIndex) * 8) + gridBit;
      //mx.setPoint(prevRow, col, bitRead(grid[prevRow][gridIndex], gridBit));
    }
  }

  for (byte i = 0; i < maxPatternBytes; i++) {
    previewGrid[i] = grid[row][i];
  }
  for (byte i = 0; i < 3; i++) {
    previewPattern[i] = patternValues[row][i];
  }
  prevLength = previewPattern[0];
  prevEvents = previewPattern[1];
  prevOffset = previewPattern[2];
}

void updateRowToEeprom(byte r) {
  for(int updatingByteToEeprom = 0; updatingByteToEeprom < 4; updatingByteToEeprom++) {
    EEPROM.update(getEepromAddress(page, r, updatingByteToEeprom), grid[r][updatingByteToEeprom]);
  }
}

void loadPageFromEeprom() {
  for(int loadingRowFromEeprom = 0; loadingRowFromEeprom < 8; loadingRowFromEeprom++) {
    for(int loadingByteFromEeprom = 0; loadingByteFromEeprom < 4; loadingByteFromEeprom++){
      grid[loadingRowFromEeprom][loadingByteFromEeprom] = EEPROM.read(getEepromAddress(page, loadingRowFromEeprom, loadingByteFromEeprom));
    }
  }
  loadedPage = page;
}

int getEepromAddress(byte page, byte row, byte rowByte) {
  //return (page * 64) + (row * 8) + byte;
  return (page << 6) + (row << 3) + rowByte;
}

void clearPattern(){

  for(byte r = 0; r < 8; r++){

    for (byte i = 0; i < maxPatternBytes; i++) {
      grid[r][i] = 0;
      previewGrid[i] = grid[r][i];
    }
    for (byte i = 0; i < 3; i++) {
      patternValues[r][i] = 0;
      previewPattern[i] = patternValues[r][i];
    }

    updateRowToEeprom(r);

  }

}

void randomisePattern(){

  for(byte r = 0; r < 8; r++){
    
    randomiseRow(r);
    /*for (byte i = 0; i < 3; i++) {
      previewPattern[i] = round(random(32));
      if(i == 0 && previewPattern[i] < 32){ // crude way to make it more likely to get longer patterns
        previewPattern[i] = round(random(32));
      }
      if(i == 0 && previewPattern[i] < 32){ // crude way to make it more likely to get longer patterns
        previewPattern[i] = round(random(32));
      }
      patternValues[r][i] = previewPattern[i];
    }

    refreshPattern(r);

    for (byte i = 0; i < maxPatternBytes; i++) {
      grid[r][i] = previewGrid[i];
    }

    updateRowToEeprom(r);*/

  }

}

void randomiseRow(byte r){
  for (byte i = 0; i < 3; i++) {
    int max = (i == 1) ? previewPattern[0] : 32; // set the max number of events to the pattern length, so it's less likely to create patterns with every beat filled
    previewPattern[i] = round(random(32));
    if(i == 0 && previewPattern[i] < 32){ // crude way to make it more likely to get longer patterns
      previewPattern[i] = round(random(32));
    }
    if(i == 0 && previewPattern[i] < 32){ // crude way to make it more likely to get longer patterns
      previewPattern[i] = round(random(32));
    }
    patternValues[r][i] = previewPattern[i];
  }

  refreshPattern(r);

  for (byte i = 0; i < maxPatternBytes; i++) {
    grid[r][i] = previewGrid[i];
  }

  updateRowToEeprom(r);
}

void stopStart(){

  if(isRunning){
    isRunning = false;
    beat = 0;
    isHalfBeat = 0;
  }else{
    isRunning = true;
  }

}

// From https://github.com/ducroq/EuclidSeqNano/blob/master/src/EuclidRhythm.cpp

bool bresenhamEuclidean(int n, int k, int o)

// Constructs a cyclic n-bit binary sequence with k 1s,
// such that the 1s are distributed as evenly as possible.
// @param n is the length of the sequence (beats or pulses)
// @param k is the number of 1s (onsets)
// @param o is the offsit (shift)
// @param s is a pointer to store the resulting sequence
// returns true on success and false on failure
// See: https://medium.com/code-music-noise/euclidean-rhythms-391d879494df

{
  if (o > n) {
    o = n;
  }
  if (k > n) {
    k = n;
  } else if (k == 0) {
    for (int i = 0; i < maxPatternLength; i++) {
      newSequence[i] = false;
    }
    return true;
  }

  boolean *c = new boolean[n];
  float slope = float(k) / float(n);
  int prev_y = -1;

  for (int i = 0; i < n; i++)
  // approximate a pixelated line and mark vertical changes
  {
    int y = floor((float(i) * slope));
    c[i] = (y != prev_y) ? true : false;
    prev_y = y;
  }
  for (int i = 0; i < maxPatternLength; i++) {

    if (i < n) {
      // shift the pattern to produce sequence
      newSequence[i] = (i + o < n) ? c[i + o] : c[i + o - n];
    } else {
      newSequence[i] = false;
    }
  }

  delete c;

  return true;
}

bool euclideanPattern(int length, int events, int offset){
  // Stores pattern in a buffer: newSequence

  // found on: https://github.com/TimMJN/Arduino-Euclidean-Rhythm-Generator/blob/main/arduino_euclidean_rhythm_generator_firmware/arduino_euclidean_rhythm_generator_firmware.ino
  // see: https://www.computermusicdesign.com/simplest-euclidean-rhythm-algorithm-explained/
  int counter;
  if (events == 0)
    counter = 0;
  else
    counter = length - events;

  for (int j = 0; j < length; j++) {
    int k = (j/* + offset*/) % length; // hack to wrap around whole length of pattern
    counter += events;
    newSequenceBeforeWrapping[k] = (counter >= length);
    counter %= length;
  }
  for (int j = length; j < 32; j++) {
    newSequenceBeforeWrapping[j] = false;
  }

  // hack to wrap around whole length of pattern
  for(int w = 0; w < 32; w++){

    int wrappedIndex = w + offset;
    if(wrappedIndex >= 32){
      wrappedIndex-= 32;
    }

    newSequence[wrappedIndex] = newSequenceBeforeWrapping[w];

  }

  return true;

}

void debugPattern() {

  if (!debug) {
    return;
  }

  Serial.println("|-------|-------|-------|-------");

  for (byte r = 0; r < 8; r++) {
    for (byte gridIndex = 0; gridIndex < 4; gridIndex++) {
      for (byte gridBit = 0; gridBit < 8; gridBit++) {
        Serial.print(bitRead(grid[r][gridIndex], 7 - gridBit) ? "O" : " ");
      }
    }
    Serial.println();
  }

  Serial.println("|-------|-------|-------|-------");
}

void refreshDisplay() {
  //opt_write(2, HIGH);

  if(millis() - lastShowedText < showTextFor){
    // don't overwrite display text for a short pause
    return;
  }

  byte gridIndex = writeDisplayCol / 8;
  byte gridBit = 7 - (writeDisplayCol % 8);

  bool showRow = millis() - lastChangedRow < showChangesFor;
  //bool showLength = millis() - lastChangedLength < showChangesFor;
  if(millis() - lastFlashedLength > flashLength){
    lengthFlashState = !lengthFlashState;
    lastFlashedLength = millis();
  }
  bool showLength = lengthFlashState;

  for (byte r = 0; r < 8; r++) {
    bool bit = false;
    if (r == row) {
      bit = bitRead(previewGrid[gridIndex], gridBit);
    } else {
      bit = bitRead(grid[r][gridIndex], gridBit);
    }
    if (writeDisplayCol == beat || (showLength && writeDisplayCol == currentChosenPatternLength) || (showRow && r == row)){//maxPatternLength - 1 - beat) {
      bit = !bit;
    }
    mx.setPoint(r, writeDisplayCol, bit);
  }

  writeDisplayCol++;
  if (writeDisplayCol >= maxPatternLength) {
    writeDisplayCol = 0;
  }

  if(millis() - lastChangedPage < showChangesFor){
    showPage();
  }/*else if(millis() - lastChangedLength < showChangesFor){
    showLength();
  }*/
  //}
  //opt_write(2, LOW);
}

void showScaleText(){
  //Serial.println(scaleNames[scale]);
  lastShowedText = millis();
  printText(0, MAX_DEVICES-1, scaleNames[scale]);
}

void showPage(){
  itoa(7 - page, digitBuffer, 10);
  digitBuffer[1] = '\0';
  printText(0, 0, digitBuffer);
}

/*void showLength(){
  itoa(nextBarPatternLength, digitBuffer, 2);
  digitBuffer[2] = '\0';
  printText(0, MAX_DEVICES-1, digitBuffer);
}*/

void printText(uint8_t modStart, uint8_t modEnd, char *pMsg)
// Print the text string to the LED matrix modules specified.
// Message area is padded with blank columns after printing.
{
  uint8_t   state = 0;
  uint8_t   curLen;
  uint16_t  showLen;
  uint8_t   cBuf[8];
  int16_t   col = ((modEnd + 1) * COL_SIZE) - 1;

  mx.control(modStart, modEnd, MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);

  do     // finite state machine to print the characters in the space available
  {
    switch(state)
    {
      case 0: // Load the next character from the font table
        // if we reached end of message, reset the message pointer
        if (*pMsg == '\0')
        {
          showLen = col - (modEnd * COL_SIZE);  // padding characters
          state = 2;
          break;
        }

        // retrieve the next character form the font file
        showLen = mx.getChar(*pMsg++, sizeof(cBuf)/sizeof(cBuf[0]), cBuf);
        curLen = 0;
        state++;
        // !! deliberately fall through to next state to start displaying

      case 1: // display the next part of the character
        mx.setColumn(31 - col--, cBuf[curLen++]);

        // done with font character, now display the space between chars
        if (curLen == showLen)
        {
          showLen = CHAR_SPACING;
          state = 2;
        }
        break;

      case 2: // initialize state for displaying empty columns
        curLen = 0;
        state++;
        // fall through

      case 3:	// display inter-character spacing or end of message padding (blank columns)
        mx.setColumn(31 - col--, 0);
        curLen++;
        if (curLen == showLen)
          state = 0;
        break;

      default:
        col = -1;   // this definitely ends the do loop
    }
  } while (col >= (modStart * COL_SIZE));

  mx.control(modStart, modEnd, MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
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
