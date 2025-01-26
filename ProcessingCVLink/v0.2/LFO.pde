import controlP5.*;
import java.time.Instant;
import java.util.*;
import java.util.concurrent.TimeUnit;
import processing.serial.*;
import processing.sound.*;
import oscP5.*;
import netP5.*;

SinOsc sineOsc1;
SinOsc sineOsc2;
SinOsc sineOsc3;
float masterVolume = 0.33; // Because there are 3 waves in total, the overall volume shouldn't go above 1
float baseFrequency = 27.5;

//WaveTypes waveTypesWindow;
WaveSpeeds waveSpeedsWindow;
WaveOffsets waveOffsetsWindow;
WaveAmplitudes waveAmplitudesWindow;
//Quantisation quantisationWindow;
//AmpMod ampModWindow;
//Sync sync;
//SampleAndHold sampleAndHold;
//FreqMod freqMod;
Repeats repeatsWindow;
ControlPanel controlPanel;
RandomControls randomControls;

Square square;
Ramp ramp;
RampInv rampInv;
Tri tri;
Sine sine;
Exp exp;

Chromatic chromatic;
Dorian dorian;
Phyrigian phyrigian;
Lydian lydian;
Mixolydian mixolydian;
Aeolian aeolian;
Locrian locrian;
LydianDominant lydianDominant;
SuperLocrian superLocrian;
Hirajoshi hirajoshi;
MinorPentatonic minorPentatonic;
MajorPentatonic majorPentatonic;
MinorBlues minorBlues;
MajorBlues majorBlues;
WholeHalfDiminished wholeHalfDiminished;
HalfWholeDiminished halfWholeDiminished;

OscP5 osc;
NetAddress supercollider;
ControlP5 cp5;
Slider abc;

boolean serialOutput = false;
boolean soundOutput = false;
boolean oscOutput = true;

Serial myPort;  // Create object from Serial class
int val;        // Data received from the serial port
boolean firstContact = false;

int speed = 750;
long lastThreadBeat = 0;

long t = 0;

float[] waveIndexes = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
float[] waveOffsets = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
float[] waveAmplitudes = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
float[] lastDisplayedWaveIndexes = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
float[] waveIncrements = { 0.012, 4.0, 12.0, 1.7, 2.0, 0.25, 8, 110, 55, 55, 55, 55, 55, 55, 8, 8 };
float[] modulatedWaveIncrements = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

int[] lastWavePoints = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
int[] secondLastWavePoints = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

int[][] waveBuffers = new int[16][4096];

int[][] euclideanPatternLengths = new int [16][10];
int[][] euclideanPatternEvents = new int [16][10];
int[][] euclideanPatternOffsets = new int [16][10];
boolean[] euclideanBeats = new boolean[128];

color[] waveColour = {
  color(255, 0, 0),
  color(0, 255, 0),
  color(0, 0, 255),
  color(255, 0, 255),
  color(0, 255, 255),
  color(255, 255, 0),
  color(127, 0, 0),
  color(0, 127, 0),
  color(255, 0, 127),
  color(127, 255, 0),
  color(127, 0, 255),
  color(255, 0, 255),
  color(127, 255, 255),
  color(255, 255, 127),
  color(127, 127, 0),
  color(0, 127, 127),
};

int[] waveTypes = {
  3,
  3,
  3,
  3,
  3,
  3,
  5,
  2,
  0,
  1,
  2,
  3,
  4,
  5,
  4,
  3
};

int[] quantise = {
  -1,
  -1,
  -1,
  -1,
  -1,
  -1,
  -1,
  -1,
  -1,
  -1,
  -1,
  -1,
  -1,
  -1,
  -1,
  -1
};

// TO DO: Add % of modulation
int[] amplitudeModulation = {
  -1,
  -1,
  -1,
  -1,
  -1,
  -1,
  -1,
  -1,
  -1,
  -1,
  -1,
  -1,
  -1,
  -1,
  -1,
  -1
};

// TO DO: Add % of modulation
int[] frequencyModulation = {
  -1,
  -1,
  -1,
  -1,
  -1,
  -1,
  -1,
  -1,
  -1,
  -1,
  -1,
  -1,
  -1,
  -1,
  -1,
  -1
};

int[] sampleAndHoldTrigger = {
  -1,
  -1,
  -1,
  -1,
  -1,
  -1,
  -1,
  -1,
  -1,
  -1,
  -1,
  -1,
  -1,
  -1,
  -1,
  -1
};
int sampledAndHeldValues[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

int[] synced = {
  -1,
  -1,
  -1,
  -1,
  -1,
  -1,
  -1,
  -1,
  -1,
  -1,
  -1,
  -1,
  -1,
  -1,
  -1,
  -1
};

int[] repeats = {
  -1,
  -1,
  -1,
  -1,
  -1,
  -1,
  -1,
  -1,
  -1,
  -1,
  -1,
  -1,
  -1,
  -1,
  -1,
  -1
};
int[] repeatCounters = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

// for debugging / test display
long lastDisplayedT = 0;
int displayedWidth = 0;

int buffersSent = 0;

void setup() {

  size(1500, 512);
  surface.setLocation(0, 0);
  noStroke();
  background(0);
  
  //waveTypesWindow = new WaveTypes();
  waveSpeedsWindow = new WaveSpeeds();
  waveOffsetsWindow = new WaveOffsets();
  waveAmplitudesWindow = new WaveAmplitudes();
  //quantisationWindow = new Quantisation();
  //ampModWindow = new AmpMod();
  //sync = new Sync();
  //sampleAndHold = new SampleAndHold();
  //freqMod = new FreqMod();
  repeatsWindow = new Repeats();
  controlPanel = new ControlPanel();
  randomControls = new RandomControls();

  cp5 = new ControlP5(this);

  square = new Square();
  ramp = new Ramp();
  rampInv = new RampInv();
  tri = new Tri();
  sine = new Sine();
  exp = new Exp();

  chromatic = new Chromatic();
  dorian = new Dorian();
  phyrigian = new Phyrigian();
  lydian = new Lydian();
  mixolydian = new Mixolydian();
  aeolian = new Aeolian();
  locrian = new Locrian();
  lydianDominant = new LydianDominant();
  superLocrian = new SuperLocrian();
  hirajoshi = new Hirajoshi();
  minorPentatonic = new MinorPentatonic();
  majorPentatonic = new MajorPentatonic();
  minorBlues = new MinorBlues();
  majorBlues = new MajorBlues();
  wholeHalfDiminished = new WholeHalfDiminished();
  halfWholeDiminished = new HalfWholeDiminished();

  // fill initial values in the modulated frequencies
  for (int i = 0; i < 16; i++) {
    modulatedWaveIncrements[i] = waveIncrements[i];
  }
  
  if(soundOutput){
    sineOsc1 = new SinOsc(this);
    sineOsc1.play();
    sineOsc2 = new SinOsc(this);
    sineOsc2.play();
    sineOsc3 = new SinOsc(this);
    sineOsc3.play();
  }

  /*refreshRandomWave(15, 8);

  euclideanPatternLengths[14][0] = 16;
  euclideanPatternEvents[14][0] = 4;
  euclideanPatternOffsets[14][0] = 0;
  euclideanPatternLengths[14][1] = 16;
  euclideanPatternEvents[14][1] = 5;
  euclideanPatternOffsets[14][1] = 0;
  for (int i = 2; i < 10; i++) {
    euclideanPatternLengths[14][i] = 0;
    euclideanPatternEvents[14][i] = 0;
    euclideanPatternOffsets[14][i] = 0;
  }
  refreshEuclideanWave(14);*/
  
  if(oscOutput){
    osc = new OscP5(this, 12000);
    supercollider = new NetAddress("127.0.0.1", 57120);
  }

  if (serialOutput) {
    String portName = Serial.list()[2];
    println(portName);
    myPort = new Serial(this, portName, 2000000);
  }
}

void serialEvent(Serial myPort) {
  // read a byte from the serial port:
  int inByte = myPort.read();
  // if this is the first byte received, and it's an A,
  // clear the serial buffer and note that you've
  // had first contact from the microcontroller.
  // Otherwise, add the incoming byte to the array:

  println("received serial: " + inByte + "\t" + buffersSent);

  buffersSent++;

  if (inByte == 'A') {
    myPort.clear();          // clear the serial port buffer
    firstContact = true;     // you've had first contact from the microcontroller
    myPort.write('C');
  } else if (inByte == 'B') {
    myPort.clear();
    
    // max buffer size on the Pico is 14000 - so 13000 is about the maximum that can be sent at one time
    // without overwriting the place that's being played
    // 1000 seems to work well here, with the Arduino requesting more data at 500 remaining samples.
    for (int m = 0; m < 1000; m++) {
      String dataString = "";
      for (int i = 0; i < 16; i++) {
        int wavePoint = getWavePoint(i);

        secondLastWavePoints[i] = lastWavePoints[i];
        lastWavePoints[i] = wavePoint;

        dataString+= wavePoint + ",";
      }
      dataString+="\n";
      myPort.write(dataString);

      t++;
      for (int i = 0; i < 16; i++) {
        waveIndexes[i]+= modulatedWaveIncrements[i];
        if (waveIndexes[i] > 4095) {
          waveIndexes[i]-= 4096.0;
          if(waveIndexes[i] < 0){
            waveIndexes[i] = 0;
          }
          repeatCounters[i]++;
        }
      }
    }
    myPort.write("\t");
  }
}

void draw() {
  
  for (int i = 0; i < 16; i++) {

    int newWavePoint = getWavePoint(i);
    
    if(soundOutput){
      if(i == 1){
        sineOsc1.amp((newWavePoint / 4095.0) * masterVolume);
      }else if(i == 2){
        sineOsc1.freq(baseFrequency * pow( 2, ((newWavePoint / 4095.0) * 60) / 12 ) );
      }else if(i == 3){
        sineOsc2.amp((newWavePoint / 4095.0) * masterVolume);
      }else if(i == 4){
        sineOsc2.freq(baseFrequency * pow( 2, ((newWavePoint / 4095.0) * 60) / 12 ) );
      }else if(i == 5){
        sineOsc3.amp((newWavePoint / 4095.0) * masterVolume);
      }else if(i == 6){
        sineOsc3.freq(baseFrequency * pow( 2, ((newWavePoint / 4095.0) * 60) / 12 ) );
      }
    }
    
    if(oscOutput){
      if(i == 1){
        OscMessage msg = new OscMessage("/sine1amp");
        msg.add((newWavePoint / 4095.0) * masterVolume);
        osc.send(msg, supercollider);
      }else if(i == 2){
        OscMessage msg = new OscMessage("/sine1freq");
        msg.add(baseFrequency * pow( 2, ((newWavePoint / 4095.0) * 60) / 12 ));
        osc.send(msg, supercollider);
      }
    }
    
    if(!serialOutput){
      secondLastWavePoints[i] = lastWavePoints[i];
      lastWavePoints[i] = newWavePoint;
    }

    stroke(waveColour[i]);

    line(displayedWidth-(serialOutput ? 10 : 1), 512 - (round(lastDisplayedWaveIndexes[i]) >> 3), displayedWidth, 512 - (newWavePoint >> 3));

    lastDisplayedWaveIndexes[i] = newWavePoint;
    
  }

  if(serialOutput){
    displayedWidth+=10;
  }else{
    displayedWidth++;
  }
  if (displayedWidth > 1500) {
    displayedWidth = 0;
    background(0);
  }

  if (!serialOutput) {
    for (int i = 0; i < 16; i++) {
      waveIndexes[i]+= modulatedWaveIncrements[i]; // waveIncrements[i];
      if (waveIndexes[i] > 4095) {
        waveIndexes[i]-= 4096.0;
        if(waveIndexes[i] < 0){
          waveIndexes[i] = 0;
        }
        repeatCounters[i]++;
      }
    }
    delay(10);
  }
}

int getWavePoint(int i) {
  
  /*// to test gate outputs
  if(i==0){
    return 1 << 6;
  }*/

  int newWavePoint = 0;

  if (synced[i] > -1 && lastWavePoints[synced[i]] > 2047 && secondLastWavePoints[synced[i]] <= 2047) {
    waveIndexes[i] = 0;
    repeatCounters[i] = 0;
  }

  if (frequencyModulation[i] > -1) {
    modulatedWaveIncrements[i] = waveIncrements[i] * ((float)lastWavePoints[frequencyModulation[i]] / 4095.0);
  }else{
    modulatedWaveIncrements[i] = waveIncrements[i];
  }
  
  switch (waveTypes[i]) {
    case -1:
      newWavePoint = waveBuffers[i][round(waveIndexes[i])];
      break;
    case 0:
      newWavePoint = square.waveTable[round(waveIndexes[i])];
      break;
    case 1:
      newWavePoint = ramp.waveTable[round(waveIndexes[i])];
      break;
    case 2:
      newWavePoint = rampInv.waveTable[round(waveIndexes[i])];
      break;
    case 3:
      newWavePoint = tri.waveTable[round(waveIndexes[i])];
      break;
    case 4:
      newWavePoint = sine.waveTable[round(waveIndexes[i])];
      break;
    case 5:
      newWavePoint = exp.waveTable[round(waveIndexes[i])];
      break;
  }

  if(repeats[i] > - 1 && repeatCounters[i] >= repeats[i]){
    waveIndexes[i] = 0;
    newWavePoint = 0;
  }

  newWavePoint = round(newWavePoint * waveAmplitudes[i]);

  if (amplitudeModulation[i] > -1) {
    newWavePoint = round(newWavePoint * ((float)lastWavePoints[amplitudeModulation[i]]) / 4095.0);
  }

  newWavePoint+= waveOffsets[i];

  if(newWavePoint < 0){
    newWavePoint = 0;
  }else if(newWavePoint > 4095){
    newWavePoint = 4095;
  }

  if (quantise[i] > -1) {
    switch (quantise[i]) {
    case 0:
      newWavePoint = chromatic.waveTable[newWavePoint];
      break;
    case 1:
      newWavePoint = dorian.waveTable[newWavePoint];
      break;
    case 2:
      newWavePoint = phyrigian.waveTable[newWavePoint];
      break;
    case 3:
      newWavePoint = lydian.waveTable[newWavePoint];
      break;
    case 4:
      newWavePoint = mixolydian.waveTable[newWavePoint];
      break;
    case 5:
      newWavePoint = aeolian.waveTable[newWavePoint];
      break;
    case 6:
      newWavePoint = locrian.waveTable[newWavePoint];
      break;
    case 7:
      newWavePoint = lydianDominant.waveTable[newWavePoint];
      break;
    case 8:
      newWavePoint = superLocrian.waveTable[newWavePoint];
      break;
    case 9:
      newWavePoint = hirajoshi.waveTable[newWavePoint];
      break;
    case 10:
      newWavePoint = minorPentatonic.waveTable[newWavePoint];
      break;
    case 11:
      newWavePoint = majorPentatonic.waveTable[newWavePoint];
      break;
    case 12:
      newWavePoint = minorBlues.waveTable[newWavePoint];
      break;
    case 13:
      newWavePoint = majorBlues.waveTable[newWavePoint];
      break;
    case 14:
      newWavePoint = wholeHalfDiminished.waveTable[newWavePoint];
      break;
    case 15:
      newWavePoint = halfWholeDiminished.waveTable[newWavePoint];
      break;
    }
  }

  if (sampleAndHoldTrigger[i] > -1) {
    if (lastWavePoints[sampleAndHoldTrigger[i]] > 2047 && secondLastWavePoints[sampleAndHoldTrigger[i]] <= 2047) {
      sampledAndHeldValues[i] = newWavePoint;
    }
    newWavePoint = sampledAndHeldValues[i];
  }
  
  // to calibrate 1V/Oct
  //return (4095/5) * 4; // Should be 4.0V if 5V is supposed to be 4095

  return newWavePoint;
}

void refreshRandomWave(int channel, int numberOfGrains) {

  int granularity = round(4095.0 / (float)numberOfGrains);

  int granularityCounter = 0;
  int randomNumber = (int)random(0, 4096);

  for (int i = 0; i < 4096; i++) {

    waveBuffers[channel][i] = randomNumber;

    granularityCounter++;
    if (granularityCounter > granularity) {
      randomNumber = (int)random(0, 4096);
      granularityCounter = 0;
    }
  }

  waveTypes[channel] = -1; // -1 = read from buffer (ie random, euclidean, etc)
}


// if this euclidean wave is used to modulate the amplitude of an envelope wave
// it can be used as an accent
// Max 10 patterns per channel, max length 128 events/grains/segments
void refreshEuclideanWave(int channel) {

  int[] cvLevels = new int[128];
  for (int i = 0; i < 128; i++) {
    cvLevels[i] = 0;
  }

  int maxLength = 0;

  for (int i = 0; i < 10; i++) {

    // If there are no events, ignore the pattern
    if (euclideanPatternLengths[channel][i] == 0 || euclideanPatternEvents[channel][i] == 0) {
      continue;
    }

    euclideanPattern(euclideanPatternLengths[channel][i], euclideanPatternEvents[channel][i], euclideanPatternOffsets[channel][i]);

    for (int j = 0; j < euclideanPatternLengths[channel][i]; j++) {
      if (euclideanBeats[j]) {
        cvLevels[j]++;
      }
    }

    if (euclideanPatternLengths[channel][i] > maxLength) {
      maxLength = euclideanPatternLengths[channel][i];
    }
  }

  int maxHeight = 0;

  for (int i = 0; i < maxLength; i++) {

    if (cvLevels[i] > maxHeight) {
      maxHeight = cvLevels[i];
    }
  }

  float scaledUnitHeight = 4095.0 / (float)maxHeight;

  for (int i = 0; i < maxLength; i++) {

    cvLevels[i] = (int)round( scaledUnitHeight * cvLevels[i] );

    if (cvLevels[i] > 4095) {
      cvLevels[i] = 4095;
    }
  }

  int granularity = round(4095.0 / (float)maxLength);

  int granularityCounter = 0;
  int patternIndex = 0;

  for (int i = 0; i < 4096; i++) {

    waveBuffers[channel][i] = cvLevels[patternIndex];

    granularityCounter++;
    if (granularityCounter > granularity) {
      if (patternIndex < maxLength - 1) {
        patternIndex++;
      }
      granularityCounter = 0;
    }
  }

  waveTypes[channel] = -1; // -1 = read from buffer (ie random, euclidean, etc)
}

void euclideanPattern(int length, int events, int offset) {
  // Stores pattern in a buffer: euclideanBeats

  // found on: https://github.com/TimMJN/Arduino-Euclidean-Rhythm-Generator/blob/main/arduino_euclidean_rhythm_generator_firmware/arduino_euclidean_rhythm_generator_firmware.ino
  // see: https://www.computermusicdesign.com/simplest-euclidean-rhythm-algorithm-explained/
  int counter;
  if (events == 0)
    counter = 0;
  else
    counter = length - events;

  for (int j = 0; j < length; j++) {
    int k = (j + offset) % length;
    counter += events;
    euclideanBeats[k] = (counter >= length);
    counter %= length;
  }
  for (int j = length; j < 128; j++) {
    euclideanBeats[j] = false;
  }
}
