byte analoguePins[] = { A7, A6, A5, A4, A3, A2 };
int analogueReadings[] = { -99, -99, -99, -99, -99, -99 };
byte analogueReadingsByte[] = { 0, 0, 0, 0, 0, 0 };
int jitterThresholds[] = { 14, 14, 14, 14, 50, 14 };
int readAnaloguePin = 0;

byte selectorValue = 0; // waveform

byte trigPin = 15; // A1
bool prevTrigState = false;

byte buttonPin = 5;
bool prevButtonState = false;
unsigned long lastButton = 0;
int debounce = 125;
volatile bool buttonTriggered = false;

byte switchPins[] = { 6, 7 };
byte switchState = 0;
// 0 = clock sync (whole numbers)
// 1 = frequency modulated one-shot
// 2 = frequency modulated

/*byte switchPins2[] = { 8, 9 };
byte switchState2 = 0;
// 0 = whole numbers
// 1 = fractions
// 2 = */

byte ledPin = 3;

int outputValue = 0;

int currentCycleCounter = 0;
int cyclesPerClock = 0;
int defaultClockCycles = 1000000;
double waveStepsPerCycle = 0;
double waveSteps = 0;
int rises = 0;
int risesPerReset = 1;

bool freeRunning = true;
bool oneShot = false;
bool wholeNumbers = true;

bool oneShotTriggered = false;

int frequency = 0;
float divider = 1.0;

//float amplitude = 1.0f;
int amplitude = 255;

int phase = 0;

const byte waves[][256] PROGMEM = {
  { 255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0 },
  { 0,	1,	2,	3,	4,	5,	6,	7,	8,	9,	10,	11,	12,	13,	14,	15,	16,	17,	18,	19,	20,	21,	22,	23,	24,	25,	26,	27,	28,	29,	30,	31,	32,	33,	34,	35,	36,	37,	38,	39,	40,	41,	42,	43,	44,	45,	46,	47,	48,	49,	50,	51,	52,	53,	54,	55,	56,	57,	58,	59,	60,	61,	62,	63,	64,	65,	66,	67,	68,	69,	70,	71,	72,	73,	74,	75,	76,	77,	78,	79,	80,	81,	82,	83,	84,	85,	86,	87,	88,	89,	90,	91,	92,	93,	94,	95,	96,	97,	98,	99,	100,	101,	102,	103,	104,	105,	106,	107,	108,	109,	110,	111,	112,	113,	114,	115,	116,	117,	118,	119,	120,	121,	122,	123,	124,	125,	126,	127,	128,	129,	130,	131,	132,	133,	134,	135,	136,	137,	138,	139,	140,	141,	142,	143,	144,	145,	146,	147,	148,	149,	150,	151,	152,	153,	154,	155,	156,	157,	158,	159,	160,	161,	162,	163,	164,	165,	166,	167,	168,	169,	170,	171,	172,	173,	174,	175,	176,	177,	178,	179,	180,	181,	182,	183,	184,	185,	186,	187,	188,	189,	190,	191,	192,	193,	194,	195,	196,	197,	198,	199,	200,	201,	202,	203,	204,	205,	206,	207,	208,	209,	210,	211,	212,	213,	214,	215,	216,	217,	218,	219,	220,	221,	222,	223,	224,	225,	226,	227,	228,	229,	230,	231,	232,	233,	234,	235,	236,	237,	238,	239,	240,	241,	242,	243,	244,	245,	246,	247,	248,	249,	250,	251,	252,	253,	254,	255 },
  { 255,	254,	253,	252,	251,	250,	249,	248,	247,	246,	245,	244,	243,	242,	241,	240,	239,	238,	237,	236,	235,	234,	233,	232,	231,	230,	229,	228,	227,	226,	225,	224,	223,	222,	221,	220,	219,	218,	217,	216,	215,	214,	213,	212,	211,	210,	209,	208,	207,	206,	205,	204,	203,	202,	201,	200,	199,	198,	197,	196,	195,	194,	193,	192,	191,	190,	189,	188,	187,	186,	185,	184,	183,	182,	181,	180,	179,	178,	177,	176,	175,	174,	173,	172,	171,	170,	169,	168,	167,	166,	165,	164,	163,	162,	161,	160,	159,	158,	157,	156,	155,	154,	153,	152,	151,	150,	149,	148,	147,	146,	145,	144,	143,	142,	141,	140,	139,	138,	137,	136,	135,	134,	133,	132,	131,	130,	129,	128,	127,	126,	125,	124,	123,	122,	121,	120,	119,	118,	117,	116,	115,	114,	113,	112,	111,	110,	109,	108,	107,	106,	105,	104,	103,	102,	101,	100,	99,	98,	97,	96,	95,	94,	93,	92,	91,	90,	89,	88,	87,	86,	85,	84,	83,	82,	81,	80,	79,	78,	77,	76,	75,	74,	73,	72,	71,	70,	69,	68,	67,	66,	65,	64,	63,	62,	61,	60,	59,	58,	57,	56,	55,	54,	53,	52,	51,	50,	49,	48,	47,	46,	45,	44,	43,	42,	41,	40,	39,	38,	37,	36,	35,	34,	33,	32,	31,	30,	29,	28,	27,	26,	25,	24,	23,	22,	21,	20,	19,	18,	17,	16,	15,	14,	13,	12,	11,	10,	9,	8,	7,	6,	5,	4,	3,	2,	1,	0 },
  { 0,	2,	4,	6,	8,	10,	12,	14,	16,	18,	20,	22,	24,	26,	28,	30,	32,	34,	36,	38,	40,	42,	44,	46,	48,	50,	52,	54,	56,	58,	60,	62,	64,	66,	68,	70,	72,	74,	76,	78,	80,	82,	84,	86,	88,	90,	92,	94,	96,	98,	100,	102,	104,	106,	108,	110,	112,	114,	116,	118,	120,	122,	124,	126,	128,	130,	132,	134,	136,	138,	140,	142,	144,	146,	148,	150,	152,	154,	156,	158,	160,	162,	164,	166,	168,	170,	172,	174,	176,	178,	180,	182,	184,	186,	188,	190,	192,	194,	196,	198,	200,	202,	204,	206,	208,	210,	212,	214,	216,	218,	220,	222,	224,	226,	228,	230,	232,	234,	236,	238,	240,	242,	244,	246,	248,	250,	252,	254,	255,	253,	251,	249,	247,	245,	243,	241,	239,	237,	235,	233,	231,	229,	227,	225,	223,	221,	219,	217,	215,	213,	211,	209,	207,	205,	203,	201,	199,	197,	195,	193,	191,	189,	187,	185,	183,	181,	179,	177,	175,	173,	171,	169,	167,	165,	163,	161,	159,	157,	155,	153,	151,	149,	147,	145,	143,	141,	139,	137,	135,	133,	131,	129,	127,	125,	123,	121,	119,	117,	115,	113,	111,	109,	107,	105,	103,	101,	99,	97,	95,	93,	91,	89,	87,	85,	83,	81,	79,	77,	75,	73,	71,	69,	67,	65,	63,	61,	59,	57,	55,	53,	51,	49,	47,	45,	43,	41,	39,	37,	35,	33,	31,	29,	27,	25,	23,	21,	19,	17,	15,	13,	11,	9,	7,	5,	3,	1 },
  { 0,	0,	0,	0,	1,	1,	1,	2,	2,	3,	4,	5,	6,	6,	7,	9,	10,	11,	12,	14,	15,	17,	18,	20,	22,	23,	25,	27,	29,	31,	33,	35,	37,	40,	42,	44,	47,	49,	52,	54,	57,	59,	62,	65,	68,	70,	73,	76,	79,	82,	85,	88,	91,	94,	97,	100,	103,	106,	109,	112,	115,	118,	122,	125,	128,	131,	134,	137,	140,	143,	146,	150,	153,	156,	159,	162,	165,	168,	171,	174,	177,	179,	182,	185,	188,	191,	193,	196,	198,	201,	204,	206,	208,	211,	213,	215,	218,	220,	222,	224,	226,	228,	230,	232,	233,	235,	237,	238,	240,	241,	242,	244,	245,	246,	247,	248,	249,	250,	251,	251,	252,	252,	253,	253,	254,	254,	254,	254,	254,	254,	254,	254,	253,	253,	252,	252,	251,	251,	250,	249,	248,	247,	246,	245,	244,	242,	241,	240,	238,	237,	235,	233,	232,	230,	228,	226,	224,	222,	220,	218,	215,	213,	211,	208,	206,	204,	201,	198,	196,	193,	191,	188,	185,	182,	179,	177,	174,	171,	168,	165,	162,	159,	156,	153,	150,	146,	143,	140,	137,	134,	131,	128,	125,	122,	118,	115,	112,	109,	106,	103,	100,	97,	94,	91,	88,	85,	82,	79,	76,	73,	70,	68,	65,	62,	59,	57,	54,	52,	49,	47,	44,	42,	40,	37,	35,	33,	31,	29,	27,	25,	23,	22,	20,	18,	17,	15,	14,	12,	11,	10,	9,	7,	6,	6,	5,	4,	3,	2,	2,	1,	1,	1,	0,	0,	0,	0 },
  { 255,	249,	243,	237,	231,	226,	220,	215,	210,	205,	200,	195,	190,	185,	181,	177,	172,	168,	164,	160,	156,	152,	149,	145,	142,	138,	135,	132,	128,	125,	122,	119,	116,	114,	111,	108,	106,	103,	101,	98,	96,	93,	91,	89,	87,	85,	83,	81,	79,	77,	75,	73,	71,	70,	68,	66,	65,	63,	62,	60,	59,	57,	56,	54,	53,	52,	51,	49,	48,	47,	46,	45,	44,	43,	42,	41,	40,	39,	38,	37,	36,	35,	34,	33,	33,	32,	31,	30,	30,	29,	28,	27,	27,	26,	25,	25,	24,	24,	23,	23,	22,	21,	21,	20,	20,	19,	19,	19,	18,	18,	17,	17,	16,	16,	16,	15,	15,	15,	14,	14,	13,	13,	13,	13,	12,	12,	12,	11,	11,	11,	11,	10,	10,	10,	10,	9,	9,	9,	9,	8,	8,	8,	8,	8,	7,	7,	7,	7,	7,	7,	6,	6,	6,	6,	6,	6,	6,	5,	5,	5,	5,	5,	5,	5,	5,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	0 },
};

byte randomWave[256];

void setup() {

  for(int i = 0; i < 6; i++){
    pinMode(analoguePins[i], INPUT);
  }
  analogReadResolution(12);

  pinMode(trigPin, INPUT);

  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(switchPins[0], INPUT_PULLUP);
  pinMode(switchPins[1], INPUT_PULLUP);
  //pinMode(switchPins2[0], INPUT_PULLUP);
  //pinMode(switchPins2[1], INPUT_PULLUP);

  pinMode(ledPin, OUTPUT);

  analogReference(DEFAULT);
  pinMode(DAC0, ANALOG);

  regenerateRandomWave();

  cyclesPerClock = defaultClockCycles; // default period 10 seconds
  calculateClockSyncWave();

  // TIMER 1 for interrupt frequency 10000 Hz:
  cli(); // stop interrupts
  TCCR1A = 0; // set entire TCCR1A register to 0
  TCCR1B = 0; // same for TCCR1B
  TCNT1  = 0; // initialize counter value to 0
  // set compare match register for 10000 Hz increments
  OCR1A = 1599; // = 16000000 / (1 * 10000) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS12, CS11 and CS10 bits for 1 prescaler
  TCCR1B |= (0 << CS12) | (0 << CS11) | (1 << CS10);
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  sei(); // allow interrupts

  // for debugging
  Serial.begin(250000);

}

ISR(TIMER1_COMPA_vect){

  bool trigState = fastioRead(15);
  //if(((trigState && !prevTrigState) || buttonTriggered) && switchState != 2){
  if((trigState && !prevTrigState) || buttonTriggered){
    oneShotTriggered = true;
    if(!freeRunning){
      cyclesPerClock = currentCycleCounter;
      calculateClockSyncWave();
      rises++;
      if(rises >= risesPerReset){
        waveSteps = 0.0f;
        rises = 0;
      }
      currentCycleCounter = 0;
    }else if(oneShot){
      waveSteps = 0.0f;
      rises = 0;
    }
    //lastTriggerRise = millis();
    //freeRunning = false;
  }
  prevTrigState = trigState;
  buttonTriggered = false;
  currentCycleCounter++;

  if(!oneShot || oneShotTriggered){
    waveSteps += waveStepsPerCycle;
  }

  if(freeRunning && waveSteps >= 254.0f){
    waveSteps = 0;
    oneShotTriggered = false;
  }

  int waveStep = round(waveSteps) % 255;

  waveStep += phase;

  if(waveStep > 255){
    waveStep -= 255;
  }

  if(oneShot && !oneShotTriggered){
    outputValue = 0;
  }else if(selectorValue < 6){
    outputValue = pgm_read_byte(&waves[selectorValue][waveStep]);
  }else if(selectorValue == 6){
    outputValue = randomWave[waveStep];
  }else if(selectorValue == 7){
    outputValue = 255;
  }

  //outputValue = round(outputValue * amplitude);
  outputValue = ((outputValue * amplitude) & 65535) >> 8;

  analogWrite(DAC0, outputValue);
  analogWrite(ledPin, outputValue);
  //if(outputValue > 255){
  //  outputValue = 0;
  //}
}

void calculateClockSyncWave(){
  waveStepsPerCycle = (256.0f / ((double)cyclesPerClock * 10)) * (wholeNumbers ? round(divider) : divider);
}

void loop() {

  /*
  // Moved to timer, for clock sync function
  bool trigState = fastioRead(15);
  if(trigState && !prevTrigState){
    triggered();
  }
  prevTrigState = trigState;*/

  bool buttonState = !fastioRead(5);
  if(buttonState && !prevButtonState && millis() - lastButton > debounce){
    buttonPress();
    lastButton = millis();
  }
  prevButtonState = buttonState;

  byte currentSwitchState = 1;
  if(!fastioRead(6)){
    currentSwitchState = 0;
  }else if(!fastioRead(7)){
    currentSwitchState = 2;
  }
  if(currentSwitchState != switchState){
    switchState = currentSwitchState;
    switchChanged();
  }

  /*byte currentSwitchState2 = 1;
  if(!fastioRead(8)){
    currentSwitchState2 = 0;
  }else if(!fastioRead(9)){
    currentSwitchState2 = 2;
  }
  if(currentSwitchState2 != switchState2){
    switchState2 = currentSwitchState2;
    switchChanged2();
  }*/

  // The analogue pins are very noisy when on USB power.
  // The grounding and noise issues are much better when
  // connected to the synth power cable.
  int analogueReading = analogRead(analoguePins[readAnaloguePin]);
  if(abs(analogueReading - analogueReadings[readAnaloguePin]) > jitterThresholds[readAnaloguePin]){
    analogueReadings[readAnaloguePin] = analogueReading;
    analogueReadingsByte[readAnaloguePin] = analogueReading >> 4;

    if(readAnaloguePin == 0 || readAnaloguePin == 4){
      frequency = analogueReadingsByte[0] + analogueReadingsByte[4] + 1;
      divider = frequency;// / 127.0f; // 127.0f / frequency;
      risesPerReset = round(1 / divider);
      //if(switchState != 2){
        calculateClockSyncWave();
      //}else{
      //  calculateFrequencyWave();
      //}
    }else if(readAnaloguePin == 1 || readAnaloguePin == 5){
      //Serial.println((analogueReadingsByte[1] + analogueReadingsByte[5]));
      //float newAmplitude = (float)(analogueReadingsByte[1] + analogueReadingsByte[5]) / 255.0f;
      //amplitude = (newAmplitude > 1.0f) ? 1.0f : newAmplitude;
      int newAmplitude = (analogueReadingsByte[1] + analogueReadingsByte[5]);
      amplitude = (newAmplitude > 255) ? 255 : newAmplitude;
    }else if(readAnaloguePin == 2){
      //decayAfterTrigger = 255.0f / analogueReadingsByte[2];
      //decayWaveAfterTrigger = analogueReadingsByte[2] >> 6;
      phase = analogueReadingsByte[2];
    }else if(readAnaloguePin == 3){
      selectorValue = analogueReadings[readAnaloguePin] >> 9;

      if(selectorValue == 6){
        regenerateRandomWave();
      }

      //Serial.print("selector: ");
      //Serial.println(selectorValue);
    }/*else{
      //Serial.print(readAnaloguePin);
      //Serial.print("\t");
      //Serial.println(analogueReadingsByte[readAnaloguePin]);
    }*/
  }
  readAnaloguePin++;
  if(readAnaloguePin >= 6){
    readAnaloguePin = 0;
  }

  //if(!freeRunning && millis() - lastTriggerRise > millisUntilFreeRunning){
  //  freeRunning = true;
  //  cyclesPerClock = defaultClockCycles; // default period 10 seconds
  //  calculateClockSyncWave();
  //}

}

void triggered(){
  //Serial.println("triggered");
}

void buttonPress(){
  //Serial.println("button");

  buttonTriggered = true;
}

void switchChanged(){
  //Serial.print("switch: ");
  //Serial.println(switchState);

  // 0 = clock sync (whole numbers)
  // 1 = frequency modulated one-shot
  // 2 = frequency modulated

  if(switchState == 0){
    freeRunning = false;
    oneShot = false;
  }else if(switchState == 1){
    cyclesPerClock = defaultClockCycles;
    freeRunning = true;
    oneShot = true;
  }else if(switchState == 2){
    cyclesPerClock = defaultClockCycles;
    freeRunning = true;
    oneShot = false;
  }

  calculateClockSyncWave();
}

void switchChanged2(){
  //Serial.print("switch2: ");
  //Serial.println(switchState2);
}

void regenerateRandomWave(){

  float steps = random(4,64);
  int newValueAt = round(256.0f / steps);

  int ticks = 0;
  byte randomValue = random(256);
  for(int i = 0; i < 256; i++){
    randomWave[i] = randomValue;
    ticks++;
    if(ticks >= newValueAt){
      ticks = 0;
      randomValue = random(256);
    }
  }
}
