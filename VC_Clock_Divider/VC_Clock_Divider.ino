/*
 * MIT License:
 * 
Copyright 2022 Nathan Birchenough

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


Schematic available at: https://github.com/Blinken-Lights/synth-modules/

A dual voltage controlled clock divider and multiplier.
Can use ratios in powers of 2 (more musical), or 1,2,3,4..,16 [chosen by switches on p2pins[] ]
 
 */

bool debug = false;

byte clkins[] = { 2, 3 };
byte clkouts[] = { 4, 5 };
byte cvinputs[] = { 0, 1 };
byte knobs[] = { 2, 3 };
byte p2pins[] = { 6, 7 };
// switch to change between factors of 2, and ascending numbers
int d[] = { 0, 0 };
bool multdiv[] = { false, false }; // false = div, true = mult
unsigned int factor[] = { 1, 1 }; // factor to multiply or divide by
//double decimalfactor[64];
bool clkinstate[] = { false, false };
unsigned long clkinperiod[] = { 0, 0 };
unsigned long clkoutperiod[] = { 0, 0 };
int clkoutcount[] = { 0, 0 };
unsigned long clkintimer[] = { 0, 0 };
unsigned long clkouttimer[] = { 0, 0 };
bool outstate[] = { true, true };
bool onetotwo = false;
unsigned long last_1_high = 0;
bool powersof2[] = { false, false };

byte cvpins[] = { A0, A1, A2, A3 };
int cvs[] = { 0, 0, 0, 0 };
int biascv = -511; // -- use this for bernoulli gate too

byte tolerance = 10;

unsigned long last_serial = 0;

void setup(){
  Serial.begin(250000);

  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);

  // optimise div calculations as multiplications
  /*for(int i = 0; i < 64; i++){
    decimalfactor[i] = 1.0 / i;
  }*/
  
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;
  OCR1A = 511;
  TCCR1B |= (1 << WGM12);
  TCCR1B |= (1 << CS10);
  TIMSK1 |= (1 << OCIE1A);
}

ISR(TIMER1_COMPA_vect){

  for(int i = 0; i < 2; i++){
    // measure rise-rise clock period
    byte j = (i == 1 && onetotwo)?0:i;
    bool cstate = opt_read(clkins[j]);
    if(cstate && !clkinstate[i]){
      clkinperiod[i] = clkintimer[i];
      clkintimer[i] = 0;
      // if multiplying, calculate multed period
      if(multdiv[i] && clkinperiod[i]>0){
        clkoutperiod[i] = clkinperiod[i] / (factor[i] * 2);
      }

      if(!multdiv[i]){
        clkoutcount[i]++;
        // if clkoutcount reaches division threshold, swap output state
        if(clkoutcount[i] >= factor[i]){
          clkoutcount[i] = 0;
          outstate[i] = !outstate[i];
          opt_write(clkouts[i], outstate[i]);
        }
      }else{
        // if multiplying, reset clkoutcount to 0
        // and only toggle out state until clkoutcount reaches factor
        clkoutcount[i] = 0;
        clkouttimer[i] = 0;
        outstate[i] = true;
        opt_write(clkouts[i], outstate[i]);
      }
      
    }else{
      clkintimer[i]++;
      clkouttimer[i]++;

      if(multdiv[i]
        && clkouttimer[i] >= clkoutperiod[i]
        && clkoutcount[i] <= (factor[i] * 2)){
        outstate[i] = !outstate[i];
        opt_write(clkouts[i], outstate[i]);
        
        clkouttimer[i] = 0;
        clkoutcount[i]++;
      }
    }
    

    clkinstate[i] = cstate;
  }
}

void loop(){

  for(int i = 0; i < 4; i++){
    int cv = analogRead(cvpins[i]);
    if(abs(cv - cvs[i]) >= tolerance){
      cvs[i] = (i<2)?(cv+biascv):cv;
    }
  }

  for(int i = 0; i < 2; i++){
    bool p2 = !opt_read(p2pins[i]);
    byte f = (cvs[cvinputs[i]] + cvs[knobs[i]]) >> (p2?6:5);
    int mid_d = p2?7:15; 
    if(f != d[i] || p2 != powersof2[i]){
      multdiv[i] = f > mid_d;
      int fc = 0;
      if(multdiv[i]){
        fc = f - mid_d;
        if(p2){
          fc = round(pow(2, (fc - 1)));
        }
      }else{
        fc = mid_d - f + 2;
        if(p2){
          fc = round(pow(2, (fc - 1)));
        }
      }
      factor[i] = max(0,min(fc, 128));
      if(multdiv[i] && clkinperiod[i]>0){
        clkoutperiod[i] = clkinperiod[i] / (factor[i] * 2);
      }
    }
    d[i] = f;
    powersof2[i] = p2;
  }

  if(opt_read(clkins[1])){
    last_1_high = millis();
    onetotwo = false;
  }else{
    onetotwo = (last_1_high == 0 || millis() - last_1_high > 30000);
  }

  if(debug && millis() - last_serial > 1000){
    last_serial = millis();

    Serial.print("cv0: ");
    Serial.print(cvs[cvinputs[0]]);
    Serial.print("\t");
    Serial.print("cv1: ");
    Serial.print(cvs[cvinputs[1]]);
    Serial.print("\t");
    Serial.print(clkinperiod[0]);
    Serial.print("\t");
    Serial.print(d[0]);
    Serial.print("\t");
    Serial.print(multdiv[0]?"mult":"div");
    Serial.print("\t");
    Serial.print(onetotwo?"1 to 2":"separate");
    Serial.print("\t");
    Serial.print(powersof2[0]?"powers0":"normal0");
    Serial.print("\t");
    Serial.print(powersof2[1]?"powers1":"normal1");
    Serial.print("\t");
    Serial.println(factor[0]);
  }
  
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
