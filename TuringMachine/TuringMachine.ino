byte clockPin = 2;

byte ledPins[] = { 11, 10, 8, 7, 6, 5, 4, 3 };

byte pwmPin = 9;

byte pulseOutPin = A1;

byte cvPin = A0;

byte chancePotPin = A7;

byte lengthPotPin = A6;

byte outputLevelPotPin = A5;

byte analogPins[] = { A0, A5, A6, A7 };
int prevAnalogueValues[] = { -1, -1, -1, -1 }; // force re-read values on first loop
byte jitterThreshold = 4;

byte writeSwitchPin1 = A3;
byte writeSwitchPin2 = A2;

volatile int randomLoop = 0;

volatile byte loopPosition = 0;

volatile byte loopLength = 0;

volatile byte chance = 0;
volatile byte chanceCV = 0;

byte outputLevel = 0;

volatile byte output = 0;

void setup() {

  randomSeed(analogRead(A2));

  pinMode(clockPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(clockPin), trigger, CHANGE);

  pinMode(writeSwitchPin1, INPUT_PULLUP);
  pinMode(writeSwitchPin2, INPUT_PULLUP);

  pinMode(cvPin, INPUT);
  pinMode(chancePotPin, INPUT);
  pinMode(lengthPotPin, INPUT);
  pinMode(outputLevelPotPin, INPUT);

  pinMode(pulseOutPin, OUTPUT);

  pinMode(pwmPin, OUTPUT);
  TCCR1A = _BV(COM1A1) | _BV(WGM10);
  TCCR1B = _BV(CS10);
  OCR1A = 0;

  for(int i = 0; i < 8; i++){
    pinMode(ledPins[i], OUTPUT);
  }

  randomLoop = random(0, 65535);

  trigger();

  //Serial.begin(250000);

}

void trigger(){

  loopPosition++;

  if(loopPosition >= loopLength){
    loopPosition = 0;
  }

  if(!opt_read(writeSwitchPin2)){
    bitSet(randomLoop, loopPosition);
  }else if(!opt_read(writeSwitchPin1)){
    bitClear(randomLoop, loopPosition);
  }else if(random(3, 255) < (chance + chanceCV)){
    bitWrite(randomLoop, loopPosition, random(0, 2));
  }

  for(int b = 0; b < 8; b++){

    int offsetLoopPosition = (loopPosition + b) % loopLength;

    bitWrite(output, b, bitRead(randomLoop, offsetLoopPosition));

    opt_write(ledPins[b], bitRead(randomLoop, offsetLoopPosition));

  }

  opt_write(pulseOutPin, bitRead(randomLoop, loopPosition));

  // To calibrate output = 4.75V
  //OCR1A = 57 * 4;

  // Normal use
  byte outNote = (output * outputLevel ) >> 10;
  if(outNote > 57){
    outNote = 57;
  }
  OCR1A = outNote << 2;

}

void loop() {

  for(byte i = 0; i < 4; i++){

    int analogueValue = analogRead(analogPins[i]);

    if(abs(analogueValue - prevAnalogueValues[i]) > jitterThreshold){

      switch (i) {

        case 0:
          chanceCV = analogueValue >> 2;
          //Serial.print("chanceCV changed: ");
          //Serial.println(chanceCV);
          break;
        case 1:
          outputLevel = analogueValue >> 2;
          //Serial.print("outputLevel changed: ");
          //Serial.println(outputLevel);
          break;
        case 2:
          loopLength = analogueValue >> 6;
          //Serial.print("loopLength changed: ");
          //Serial.println(loopLength);
          break;
        case 3:
          chance = analogueValue >> 2;
          //Serial.print("chance changed: ");
          //Serial.println(chance);
          break;

      }

      prevAnalogueValues[i] = analogueValue;

    }

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
