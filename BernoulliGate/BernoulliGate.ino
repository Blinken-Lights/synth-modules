byte cvPotPins[] = { A7, A6 };

byte cvPins[] = { A5, A4 };

byte cvLedPins[] = { 3, 5 };

byte trigPins[] = { 4, 6 };

byte outputPins[2][2] = {
  { 7, 9 },
  { 8, 10 }
};

bool prevTrigPinStates[] = { false, false };

int randomCVs[] = { 0, 0 };

void setup() {
  
  randomSeed(analogRead(A0));

  for(int i = 0; i < 2; i++){

    pinMode(cvPotPins[i], INPUT);
    pinMode(cvPins[i], INPUT);
    pinMode(cvLedPins[i], OUTPUT);
    pinMode(trigPins[i], INPUT);

    for(int j = 0; j < 2; j++){
      pinMode(outputPins[i][j], OUTPUT);
    }

  }

}

void loop() {
  
  for(int i = 0; i < 2; i++){

    bool trigPinState = opt_read(trigPins[i]);

    if(trigPinState && !prevTrigPinStates[i]){
      // LOW to HIGH transition
      byte chosenPin = random(1023) < randomCVs[i];
      byte unchosenPin = !chosenPin;

      opt_write(outputPins[i][unchosenPin], LOW);
      opt_write(outputPins[i][chosenPin], HIGH);

    }else if(!trigPinState && prevTrigPinStates[i]){
      // HIGH to LOW transition
      for(int j = 0; j < 2; j++){
        opt_write(outputPins[i][j], LOW);
      }
    }

    prevTrigPinStates[i] = trigPinState;

    randomCVs[i] = analogRead(cvPotPins[i]) + analogRead(cvPins[i]);

    int scaledRandomCV = randomCVs[i] >> 2;
    if(scaledRandomCV > 255){
      scaledRandomCV = 255;
    }

    analogWrite(cvLedPins[i], scaledRandomCV);

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
