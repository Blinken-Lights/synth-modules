#include <TimerOne.h>
#include <MIDI.h>

// ASCIISynth SID - MIDI integration
// https://github.com/Blinken-Lights/synth-modules/tree/master/SID-MIDI

MIDI_CREATE_DEFAULT_INSTANCE();

const byte o2  = 9;
const byte cs = 10;
const byte reset = 11;

// Save states of registers on SID that contain more than one parameter,
// so need to be written multiple times changing one parameter but retaining
// the states of other parameters
byte conreg[] = { 0, 0, 0 };
byte modevol_register = B00011111;
byte resfilt_register = 0;
byte resonance;

const byte analogueTolerance = 10;
const byte analoguePins[] = { A5, A6, A7 };
int analogueReadings[] = { 9999, 9999, 9999 }; // set high to force a refresh on first loop
const byte envKnob = 0; // A5
const byte waveStackKnob = 1; // A6
const byte pulseWidthKnob = 2; // A7

const byte polyUnisonSwitch0 = 12;
const byte polyUnisonSwitch1 = 13;
byte polyUnisonSwitchState = 0;
bool polyphonic = false;
bool unison = false;
// These channels are used if separate channels mode is chosen
byte midiChannels[] = { 1, 2, 3 }; // TO DO: is this 0 indexed?
byte midiChanneltoSIDChannel[] = { 0, 0, 0 }; // this is filled in setup()
// This channel is used if polyphonic mode is chosen
byte polyUnisonChannel = 1;
byte currentPolySidChannel = 0;

byte channelNotes[] = { 255, 255, 255 }; // 255 = off
byte channelVelocities[] = { 255, 255, 255 }; // 255 = off
// All env params: 0 - 15
byte channelAttacks[] = { 7, 7, 7 };
byte channelDecays[] = { 7, 7, 7 };
byte channelSustains[] = { 7, 7, 7 };
byte channelReleases[] = { 7, 7, 7 };
int channelPulseWidths[] = { 2048, 2048, 2048 };
byte channelWaves[] = { 1, 1, 1 };

bool portamentoStates[] = { false, false, false };
byte portamentoTimes[] = { 0, 0, 0 };
int currentFrequencies[] = { 0, 0, 0 };
int targetFrequencies[] = { 0, 0, 0 };
unsigned long lastCheckedPortamento[] = { 0, 0, 0 };

/* Code used to generate these Fn values:
  for(byte n = 0; n < 128; n++){

    Serial.print(n);
    Serial.print("\t");

    byte note = n;
    if(n > 118){
      note = n-24;
    }else if(n > 106){
      note = n-12;
    }

    float midiF = 440.0f * pow(2, (((float)note-69.0f)/12.0f));

    Serial.print(midiF);
    Serial.print("\t");

    // fout = fn * 0.0596
    double multiplier = 16.7785234899;
    unsigned int fn = midiF * multiplier;
    Serial.print(fn);
    Serial.print("\t");


    Serial.println();

  }
*/

const unsigned int note_to_sid[] PROGMEM = { 137,145,153,163,172,183,193,205,217,230,244,258,274,290,307,326,345,366,387,411,435,461,488,517,548,581,615,652,691,732,775,822,871,922,977,1035,1097,1162,1231,1305,1382,1464,1551,1644,1742,1845,1955,2071,2194,2325,2463,2610,2765,2929,3103,3288,3484,3691,3910,4143,4389,4650,4927,5220,5530,5859,6207,6577,6968,7382,7821,8286,8779,9301,9854,10440,11061,11719,12415,13154,13936,14765,15643,16573,17558,18602,19709,20881,22122,23438,24831,26308,27872,29530,31286,33146,35117,37205,39418,41762,44245,46876,49663,52616,55745,59060,62572,33146,35117,37205,39418,41762,44245,46876,49663,52616,55745,59060,62572,33146,35117,37205,39418,41762,44245,46876,49663,52616 };

// in unison and polyphonic modes, midi channels CCs linked to voices 1, 2, 3 can still change those individual voices
void handleControlChange(byte channel, byte number, byte value){
  if(channel != midiChannels[0] && channel != midiChannels[1] && channel != midiChannels[2]){
    return;
  }

  byte sidChannel = midiChanneltoSIDChannel[channel];

  switch (number) {
    case 0: // wave single-channel
      if(value > 3){
        value = 3;
      }
      channelWaves[sidChannel] = value;
      updateWave(sidChannel);
      break;

    case 1: // sync single-channel
      setSync(sidChannel, (value > 64));
      break;

    case 2: // ringmod single-channel
      setRingMod(sidChannel, (value > 64));
      break;

    case 3: // attack single-channel
      channelAttacks[sidChannel] = value >> 3;
      updateEnv(sidChannel);
      break;

    case 4: // release single-channel
      channelReleases[sidChannel] = value >> 3;
      updateEnv(sidChannel);
      break;

    case 5: // sustain single-channel
      channelSustains[sidChannel] = value >> 3;
      updateEnv(sidChannel);
      break;

    case 6: // release single-channel
      channelSustains[sidChannel] = value >> 3;
      updateEnv(sidChannel);
      break;

    case 7: // pulse width single-channel
      channelPulseWidths[sidChannel] = value << 5;
      updatePW(sidChannel);
      break;

    case 8: // filter on single-channel
      setFilterState(sidChannel, (value > 64));
      break;

    case 9: // portamento on single-channel
      setPortamentoState(sidChannel, (value > 64));
      break;

    case 10: // portamento time single-channel
      setPortamentoTime(sidChannel, value);
      break;

    // CCs for registers that affect all voices, IE paraphonic
    
    case 11: // filter frequency
      setFilterFreq(value << 4);
      break;

    case 12: // filter resonance
      resonance = value >> 3;
      setResFiltRegister();
      break;

    case 13: // filter mode
      bool lp = false;
      bool bp = false;
      bool hp = false;
      if(value < 42){
        lp = true;
        bp = false;
        hp = false;
      }else if(value < 84){
        lp = false;
        bp = true;
        hp = false;
      }else{
        lp = false;
        bp = false;
        hp = true;
      }
      setFiltermode(hp, bp, lp);
      break;

    case 14: // turn voice 3 off
      setVoice3State((value > 64));
      break;

    case 15: // master volume
      setVolume(value >> 3);
      break;

    // global CCs affecting all voices at once

    case 20: // wave all-channel
      if(value > 3){
        value = 3;
      }
      for(byte i = 0; i < 3; i++){
        channelWaves[i] = value;
        updateWave(i);
      }
      break;

    case 21: // sync all-channel
      for(byte i = 0; i < 3; i++){
        setSync(i, (value > 64));
      }
      break;

    case 22: // ringmod all-channel
      for(byte i = 0; i < 3; i++){
        setRingMod(i, (value > 64));
      }
      break;

    case 23: // attack all-channel
      for(byte i = 0; i < 3; i++){
        channelAttacks[i] = value >> 3;
        updateEnv(i);
      }
      break;

    case 24: // release all-channel
      for(byte i = 0; i < 3; i++){
        channelReleases[i] = value >> 3;
        updateEnv(i);
      }
      break;

    case 25: // sustain all-channel
      for(byte i = 0; i < 3; i++){
        channelSustains[i] = value >> 3;
        updateEnv(i);
      }
      break;

    case 26: // release all-channel
      for(byte i = 0; i < 3; i++){
        channelSustains[i] = value >> 3;
        updateEnv(i);
      }
      break;

    case 27: // pulse width all-channel
      for(byte i = 0; i < 3; i++){
        channelPulseWidths[i] = value << 5;
        updatePW(i);
      }
      break;

    case 28: // filter on all-channel
      setAllFilterStates((value > 64));
      break;

    case 29: // portamento on all-channel
      for(byte i = 0; i < 3; i++){
        setPortamentoState(i, (value > 64));
      }
      break;

    case 30: // portamento time single-channel
      for(byte i = 0; i < 3; i++){
        setPortamentoTime(i, value);
      }
      break;

  }
  
}

void handleNoteOn(byte channel, byte inNote, byte inVelocity){
  if(ignoreChannel(channel)){
    return;
  }

  if(inVelocity == 0){
    handleNoteOff(channel, inNote, inVelocity);
    return;
  }

  if(unison){

    for(byte i = 0; i < 3; i++){
      noteOn(i, inNote, inVelocity);
    }

  }else if(polyphonic){

    bool foundEmptyChannel = false;
    for(byte i = 0; i < 3; i++){
      if(channelNotes[i] == 255){
        currentPolySidChannel = i;
        foundEmptyChannel = true;
        break;
      }
    }
    if(!foundEmptyChannel){
      noteOff(currentPolySidChannel);
    }
    noteOn(currentPolySidChannel, inNote, inVelocity);

    currentPolySidChannel++;
    if(currentPolySidChannel >= 3){
      currentPolySidChannel = 0;
    }

  }else{
    noteOn(midiChanneltoSIDChannel[channel], inNote, inVelocity);
  }

}

void handleNoteOff(byte channel, byte inNote, byte inVelocity){
  if(ignoreChannel(channel)){
    return;
  }

  if(unison){

    for(byte i = 0; i < 3; i++){
      noteOff(i);
    }

  }else if(polyphonic){
    
    for(byte i = 0; i < 3; i++){
      if(channelNotes[i] == inNote){
        noteOff(i);
        break;
      }
    }

  }else{
    noteOff(midiChanneltoSIDChannel[channel]);
  }
    
}

bool ignoreChannel(byte channel){
  return ((polyphonic || unison) && channel != polyUnisonChannel) || (!(polyphonic || unison) && channel != midiChannels[0] && channel != midiChannels[1] && channel != midiChannels[2]);
}

// When mode is changed between polyphonic / separate channels, turn off poly notes on the SID
void resetNotes(){

  for(byte i = 0; i < 3; i++){
    if(channelNotes[i] < 255){
      gateOff(i);
      channelNotes[i] = 255;
      channelVelocities[i] = 0;
    }
  }

}

void noteOn(byte sidChannel, byte note, byte velocity){

  channelNotes[sidChannel] = note;
  channelVelocities[sidChannel] = velocity;
  channelSustains[sidChannel] = channelVelocities[sidChannel] >> 4;
  setMIDINote(channelNotes[sidChannel], sidChannel);
  updateEnv(sidChannel);
  gateOn(sidChannel);

}

void noteOff(byte sidChannel){

  channelNotes[sidChannel] = 255;
  channelVelocities[sidChannel] = 0;
  gateOff(sidChannel);

}

void setup() {

  Timer1.initialize(1); // period in us   
  Timer1.pwm(/* pin */ 9, /* duty 0-1023 = 0-100% */ 511);

  pinMode(o2, OUTPUT);
  pinMode(cs, OUTPUT);
  pinMode(reset, OUTPUT);
  digitalWrite(reset, HIGH);

  // data pins
  pinMode(8, OUTPUT);
  pinMode(1, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  // address pins
  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode(A4, OUTPUT);

  digitalWrite(cs, HIGH);

  sidReset();
  
  sidWrite(24, modevol_register);

  setFreq(923, 0);
  setFreq(1845, 1);
  setFreq(3691, 2);

  for(byte i = 0; i < 3; i++){
    updatePW(i);
    updateWave(i);
    updateEnv(i);
  }

  pinMode(polyUnisonSwitch0, INPUT_PULLUP);
  pinMode(polyUnisonSwitch1, INPUT_PULLUP);

  /*// debugging..
  gateOn(0);
  gateOn(1);
  gateOn(2);
  delay(1000);
  gateOff(0);
  gateOff(1);
  gateOff(2);
  // end debugging ..*/

  // to interpret MIDI messages when in separate channels mode
  for(int i = 0; i < 3; i++){
    midiChanneltoSIDChannel[midiChannels[i]] = i;
  }

  MIDI.setHandleNoteOn(handleNoteOn);
  MIDI.setHandleNoteOff(handleNoteOff);
  MIDI.setHandleControlChange(handleControlChange);
  MIDI.begin(MIDI_CHANNEL_OMNI);
  MIDI.turnThruOff();

}

void loop() {

  MIDI.read();

  byte currentPolyUnisonSwitchState = (opt_read(polyUnisonSwitch1) << 1) | opt_read(polyUnisonSwitch0);
  if(currentPolyUnisonSwitchState != polyUnisonSwitchState){
    switch (currentPolyUnisonSwitchState) {
      case 2:
        polyphonic = false;
        unison = true;
        break;

      case 3:
        polyphonic = true;
        unison = false;
        break;

      case 1:
        polyphonic = false;
        unison = false;
        break;
    }

    resetNotes();
  }
  polyUnisonSwitchState = currentPolyUnisonSwitchState;

  readKnobs();

  checkPortamentos();

}

void readKnobs(){

  for(byte i = 0; i < 3; i++){
    int value = analogRead(analoguePins[i]);
    if(abs(value - analogueReadings[i]) < analogueTolerance){
      continue;
    }

    analogueReadings[i] = value;

    switch (i){
      case envKnob:
        setAllEnvs(value);
        break;

      case waveStackKnob:
        setWaveStack(value);
        break;

      case pulseWidthKnob:
        setAllPulseWidths(value);
        break;
    }

  }

}

void checkPortamentos(){

  unsigned long m = millis();

  for(byte i = 0; i < 3; i++){

    if(!portamentoStates[i]
      || currentFrequencies[i] == targetFrequencies[i]
      || m - lastCheckedPortamento[i] < portamentoTimes[i]){
      continue;
    }

    int newFrequency = (currentFrequencies[i] > targetFrequencies[i]) ? currentFrequencies[i] - 1 : currentFrequencies[i] + 1 ;

    setFreq(newFrequency, i);

    lastCheckedPortamento[i] = m;

  }

}

void setFilterState(byte sidChannel, bool filterState){
  bitWrite(resfilt_register, sidChannel, filterState);
  sidWrite(23, resfilt_register);
}

void setAllFilterStates(bool filterState){
  resfilt_register &= filterState ? B00000111 : B00000000;
  sidWrite(23, resfilt_register);
}

void setResFiltRegister(){
  resfilt_register &= B00001111;
  resfilt_register |= resonance << 4;
  sidWrite(23, resfilt_register);
}

void sidReset(){
  opt_write(reset, LOW);
  delay(1);
  opt_write(reset, HIGH);
}

void sidWrite(byte addressbyte, byte databyte){

  PORTC = addressbyte;
  opt_write(cs, LOW);
  PORTD = databyte;
  bitWrite(PORTB, 0, bitRead(databyte, 0));

  //delayMicroseconds(200); // was 2 // set CS LOW
  //opt_write(cs, LOW);
  //PORTB &= (~(1 << 2));
  delayMicroseconds(2); // set CS HIGH again
  opt_write(cs, HIGH);
  delayMicroseconds(2); // set CS HIGH again
  //PORTB |= (1 << 2);
    
}

void setEnv(byte a, byte d, byte s, byte r, byte c){
    byte reg = c * 7;
    sidWrite((reg+5), ((a & B00001111)<<4) | (d & B00001111));
    sidWrite((reg+6), ((s & B00001111)<<4) | (r & B00001111));
}

void updateEnv(byte sidChannel){
  setEnv(channelAttacks[sidChannel], channelDecays[sidChannel], channelSustains[sidChannel], channelReleases[sidChannel], sidChannel);
}

void setAllEnvs(int value){

  byte newAttack = 0;
  byte newRelease = 0;

  if(value < 500){ // attack envelope
    newAttack = (500 - value) >> 5;
  }else if(value < 522){ // attack/release envelope
    // fastest attack and release
  }else{ // release envelope
    newRelease = (value - 522) >> 5;
  }

  for(byte i = 0; i < 3; i++){
    channelAttacks[i] = newAttack;
    channelDecays[i] = newRelease;
    channelReleases[i] = newRelease;
    updateEnv(i);
  }

}

void setFreq(int f, byte c){
  byte reg = c * 7;
  sidWrite(reg,     (f & B11111111));
  sidWrite((reg+1), ((f >> 8) & B11111111));
  
  currentFrequencies[c] = f;
}

// TO DO: Can this implement portamento????
void setMIDINote(byte note, byte sidChannel){
  int sidNote = pgm_read_word_near(note_to_sid + note);

  targetFrequencies[sidChannel] = sidNote;

  if(!portamentoStates[sidChannel]){
    setFreq(sidNote, sidChannel);
  }
}

void setFilterFreq(int f){
    sidWrite(21, (f & B00000111));
    sidWrite(22, ((f >> 3) & B11111111));
}

// 0 = Tri, 1 = Saw, 2 = Pulse, 3 = Noise
void setWave(byte wave, byte c){
  byte reg = c * 7;
  conreg[c] = (1 << (wave + 4)) | (conreg[c] & B00001111);

  sidWrite((reg+4), conreg[c]);
}

void updateWave(byte sidChannel){
  setWave(channelWaves[sidChannel], sidChannel);
}

void setSync(byte sidChannel, bool syncState){
  byte reg = sidChannel * 7;
  bitWrite(conreg[sidChannel], 1, syncState);
  sidWrite((reg+4), conreg[sidChannel]);
}

void setRingMod(byte sidChannel, bool ringModState){
  byte reg = sidChannel * 7;
  bitWrite(conreg[sidChannel], 2, ringModState);
  sidWrite((reg+4), conreg[sidChannel]);
}

void setWaveStack(int value){

  byte permutation = value >> 3; // 0 - 127

  for(byte i = 0; i < 3; i++){
    channelWaves[i] = (permutation >> ((i * 2) + 1)) & B00000011;
    bool sync = (permutation >> 6) & B00000001;
    bool ringmod = ((permutation >> 1) & B00000001) && channelWaves[i] == 0;
    // portamento could be another flag set here..?
    bitWrite(conreg[i], 1, sync);
    bitWrite(conreg[i], 2, ringmod);
    updateWave(i);
  }

}

void setPW(int pw, byte c){
    byte reg = c * 7;

    byte lowByte =  (pw & B11111111);
    byte highByte =  ((pw >> 8) & B00001111);

    sidWrite((reg+2), lowByte);
    sidWrite((reg+3), highByte);
}

void updatePW(byte sidChannel){
  setPW(channelPulseWidths[sidChannel], sidChannel);
}

void setAllPulseWidths(int value){
  int newPulseWidth = value << 2; // 0 - 4095

  for(byte i = 0; i < 3; i++){
    channelPulseWidths[i] = newPulseWidth;
    updatePW(i);
  }
}

void setPortamentoState(byte sidChannel, bool portamentoState){
  portamentoStates[sidChannel] = portamentoState;
}

void setPortamentoTime(byte sidChannel, byte portamentoTime){
  portamentoTimes[sidChannel] = portamentoTime;
}

void setFiltermode(bool hp, bool bp, bool lp){
  bitWrite(modevol_register, 4, lp);
  bitWrite(modevol_register, 5, bp);
  bitWrite(modevol_register, 6, hp);
  sidWrite(24, modevol_register);
}

void setVoice3State(bool voice3State){
  bitWrite(modevol_register, 7, voice3State);
  sidWrite(24, modevol_register);
}

void setVolume(byte volume){
  modevol_register &= B11110000;
  modevol_register |= (volume & B00001111);
  sidWrite(24, modevol_register);
}

void gateOn(byte c){
    byte reg = c * 7;
    conreg[c] = conreg[c] | B00000001;
    
    sidWrite((reg+4), conreg[c]);
}

void gateOff(byte c){
    byte reg = c * 7;
    conreg[c] = conreg[c] & B11111110;

    sidWrite((reg+4), conreg[c]);
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