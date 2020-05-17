#include <SPI.h>
# dependency: https://github.com/olikraus/u8glib
#include "U8glib.h"

U8GLIB_SH1106_128X64 u8g(10, 9,  8);

const byte master_switch = A0; // PORTC 0
const byte clock_out = A5; // PORTC 5
const byte clock_in = 2;
const byte sync_in = 3;
const byte rotary_pin = A7;
byte rotary_value = 0;
byte prev_rotary_value = 0;
const byte step_pin = A6;
byte step_value = 0;
byte prev_step_value = 0;
const byte pattern_pin = A3;
byte pattern_value = 0;
byte prev_pattern_value = 0;

byte editing_parameter = 0;

const byte enc_clock = 4;
const byte enc_data = 5;
const byte enc_switch = 6;
bool prev_clock_state = 1;
unsigned long last_encoder_read;

const byte toggle_pattern = 7;
bool toggle_state;
unsigned long last_toggled_pattern;

enum {BufSize=9};
char buf[BufSize];
char string_buffer[20];
bool change_detected = true;

byte circlex[2] = { 30,  90 };
byte circley[2] = { 32,  32 };
byte pointx[8] = {   0,  14, 20, 14,  0, -14, -20, -14 };
byte pointy[8] = { -20, -14,  0, 14, 20,  14,   0, -14 };
byte poishx[8] = {   0,   7, 10,  7,  0,  -7, -10,  -7 };
byte poishy[8] = { -10,  -7,  0,  7, 10,   7,   0,  -7 };

// for 16 patterns
int editing_pattern;
int editing_index;
bool play_pattern[16] = { false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false };
int beat_number[16];
int pattern[16][8];
int resolution[16] = { 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4 };
int sequence_length[16] = { 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8 };
int sequence_index[16];
int root_note[16] = { 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45 };
int midi_root_note[16] = { 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255 };
int octave_offset[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
int random_offset[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
bool d[16] = { true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true };
int swing[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
byte channel[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
byte modifier[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
bool modified_by[16][16];
byte previous_note[16];
volatile bool clock_changed;
int clock_tick_counter;

char *notetypenames[9] = // 255 - index
{
  "-", // rest
  "R", // random
  "r", // slightly random
  "V", // very random
  "O", // random octave change
  "^", // toggle octave up
  "`", // toggle octave down
  "<", // toggle direction
  "!", // stop pattern (so use as one-shot pattern, triggered by another pattern / button
};
// pattern memory banks saved and triggered using switches and external gates

char *modifiernames[7] =
{
  "M", // play on MIDI channel
  "+", // add to note of target pattern
  "-", // subtract from note of target pattern
  "L", // override length of sequence
  "R", // override resolution
  "T", // toggle pattern on/off
  "S"  // swing
};

//global parameters
int bpm = 240;
float clock_hz = 0.0;
int key;
int scale;

char *keynames[12] =
{
  "A ",
  "A#",
  "B ",
  "C ",
  "C#",
  "D ",
  "D#",
  "E ",
  "F ",
  "F#",
  "G ",
  "G#"
};

const byte writing_channel = 0;
//MIDI Out
const byte note_off                 = 0 << 4;
const byte note_on                  = 1 << 4;
const byte velocity_on              = 90;
const byte velocity_off             = 0;
byte status_byte;
byte data_byte1;
byte data_byte2;
byte playing_note[16];
byte midi_drums[] = { 35, 38, 45, 46 };
//MIDI IN
bool mi_midi_gate;
byte mi_input_byte;
byte mi_status_byte;
byte mi_data_byte1;
byte mi_data_byte2;
byte mi_channel;
byte mi_key;
byte mi_prev_key;
bool mi_channel_status;
bool mi_prev_channel_status;

byte current_note_on = 255;

byte mi_code;

const byte mi_note_off                 = 0;
const byte mi_note_on                  = 1;
const byte poly_pressure            = 2;
const byte controller_change        = 3;
const byte program_change           = 4;
const byte channel_pressure_change  = 5;
const byte pitch_bend_change        = 6;

// channel mode messages
const byte channel_volume           = 7;
// channel mode messages that cause all notes to switch off
const byte sound_off                = 120;
const byte reset_controllers        = 121;
const byte all_notes_off            = 123;
const byte omni_on                  = 124;
const byte omni_off                 = 125;
const byte mono_on                  = 126;
const byte poly_on                  = 127;

const byte clock_code               = 248;
const byte stop_clock               = 252;
const byte reset_all                = 255;
// \midi variables

char *scalenames[7] =
{
  "Cr",
  "Ma",
  "Mi",
  "Ha",
  "Do",
  "Hi",
  "Ch"
};

const byte scales[7][128] PROGMEM = {
    {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127 },
        {16,18,8,9,11,13,14,16,18,8,9,11,13,14,16,18,8,9,11,1,2,4,6,8,9,11,13,14,16,18,20,21,23,25,26,28,30,32,33,35,37,38,40,42,44,45,47,49,50,52,54,56,57,59,61,62,64,66,68,69,71,73,74,76,78,80,81,83,85,86,88,90,92,93,95,97,98,100,102,104,105,107,109,110,112,114,116,117,119,121,122,124,126,116,117,119,109,110,112,114,116,117,119,109,110,112,114,116,117,119,109,110,112,114,116,117,119,109,110,112,114,116,117,119,109,110,112,114 },
        {16,17,7,9,11,12,14,16,17,7,9,11,12,14,16,17,7,9,11,0,2,4,5,7,9,11,12,14,16,17,19,21,23,24,26,28,29,31,33,35,36,38,40,41,43,45,47,48,50,52,53,55,57,59,60,62,64,65,67,69,71,72,74,76,77,79,81,83,84,86,88,89,91,93,95,96,98,100,101,103,105,107,108,110,112,113,115,117,119,120,122,124,125,127,117,119,120,110,112,113,115,117,119,120,110,112,113,115,117,119,120,110,112,113,115,117,119,120,110,112,113,115,117,119,120,110,112,113 },
        {16,17,8,9,11,12,14,16,17,8,9,11,12,14,16,17,8,9,11,0,2,4,5,8,9,11,12,14,16,17,20,21,23,24,26,28,29,32,33,35,36,38,40,41,44,45,47,48,50,52,53,56,57,59,60,62,64,65,68,69,71,72,74,76,77,80,81,83,84,86,88,89,92,93,95,96,98,100,101,104,105,107,108,110,112,113,116,117,119,120,122,124,125,116,117,119,120,110,112,113,116,117,119,120,110,112,113,116,117,119,120,110,112,113,116,117,119,120,110,112,113,116,117,119,120,110,112,113 },
        {16,18,7,9,11,12,14,16,18,7,9,11,12,14,16,18,7,9,11,0,2,4,6,7,9,11,12,14,16,18,19,21,23,24,26,28,30,31,33,35,36,38,40,42,43,45,47,48,50,52,54,55,57,59,60,62,64,66,67,69,71,72,74,76,78,79,81,83,84,86,88,90,91,93,95,96,98,100,102,103,105,107,108,110,112,114,115,117,119,120,122,124,126,127,117,119,120,110,112,114,115,117,119,120,110,112,114,115,117,119,120,110,112,114,115,117,119,120,110,112,114,115,117,119,120,110,112,114 },
        {9,11,12,16,17,9,11,12,16,17,9,11,12,16,17,9,11,12,16,17,9,11,12,16,17,9,11,0,4,5,9,11,12,16,17,21,23,24,28,29,33,35,36,40,41,45,47,48,52,53,57,59,60,64,65,69,71,72,76,77,81,83,84,88,89,93,95,96,100,101,105,107,108,112,113,117,119,120,124,125,117,119,120,112,113,117,119,120,112,113,117,119,120,112,113,117,119,120,112,113,117,119,120,112,113,117,119,120,112,113,117,119,120,112,113,117,119,120,112,113,117,119,120,112,113,117,119,120 },
        {9,11,13,16,18,9,11,13,16,18,9,11,13,16,18,9,11,13,16,18,9,11,13,16,18,9,11,1,4,6,9,11,13,16,18,21,23,25,28,30,33,35,37,40,42,45,47,49,52,54,57,59,61,64,66,69,71,73,76,78,81,83,85,88,90,93,95,97,100,102,105,107,109,112,114,117,119,121,124,126,117,119,109,112,114,117,119,109,112,114,117,119,109,112,114,117,119,109,112,114,117,119,109,112,114,117,119,109,112,114,117,119,109,112,114,117,119,109,112,114,117,119,109,112,114,117,119,109 },
            };

void setup(void){

  Serial.begin(31250);

  pinMode(enc_clock, INPUT_PULLUP);
  pinMode(enc_data, INPUT_PULLUP);
  pinMode(enc_switch, INPUT_PULLUP);

  pinMode(toggle_pattern, INPUT_PULLUP);
  
  pinMode(master_switch, INPUT_PULLUP);

  pinMode(clock_in, INPUT);
  attachInterrupt(digitalPinToInterrupt(clock_in), clock_tick, FALLING);
  pinMode(clock_out, OUTPUT);
  pinMode(sync_in, INPUT);
  attachInterrupt(digitalPinToInterrupt(sync_in), sync, RISING);
  
  delay(1000);
  
  u8g.setFont(u8g_font_profont10);

  set_clock_interrupt();

  change_detected = true;
}

ISR(TIMER1_COMPA_vect){
  clock_tick();
}

void set_clock_interrupt(){

  if(bpm <= 0){
    bpm = 1;
  }

  clock_hz = (float)bpm / 60.0;
  
  unsigned long ocr_value = round(16000000.0 / (256.0 * clock_hz)) - 1;

  bool use_1024 = false;
  if(ocr_value >= 65535){
      use_1024 = true;

      ocr_value = round(16000000.0 / (1024.0 * clock_hz)) - 1;
  }

  if(ocr_value >= 65535){
      ocr_value = 65535;

      clock_hz = (16000000.0 / (float)(ocr_value + 1)) / 1024.0;

      bpm = clock_hz * 60;
  }

  // TIMER 1          // for interrupt frequency 2 Hz / 120bpm: //change value of OCR1A to change internal tempo
  cli(); // stop interrupts
  TCCR1A = 0; // set entire TCCR1A register to 0
  TCCR1B = 0; // same for TCCR1B
  TCNT1  = 0; // initialize counter value to 0
  // set compare match register for 2 Hz increments
  OCR1A = ocr_value;
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  if(use_1024){
      // Set CS12, CS11 and CS10 bits for 1024 prescaler
      TCCR1B |= (1 << CS12) | (0 << CS11) | (1 << CS10);
  }else{
      // Set CS12, CS11 and CS10 bits for 256 prescaler
      TCCR1B |= (1 << CS12) | (0 << CS11) | (0 << CS10);
  }
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  sei(); // allow interrupts
}

void loop(void){  

  if (Serial.available() > 0) {
      
    mi_input_byte = Serial.read();
    
    if(bitRead(mi_input_byte, 7)){
        mi_status_byte = mi_input_byte;
        mi_code = (mi_status_byte & B01110000) >> 4;
        mi_channel = mi_status_byte & B00001111;
        mi_data_byte1 = 0;
        mi_data_byte2 = 0;

        //reset all
        if(mi_status_byte == reset_all){
            mi_key = 0;
            //reset all
            reset_sequence();
        }
        
    }else if(mi_data_byte1==0){
        mi_data_byte1 = mi_input_byte;

        //if status byte matches single data byte messages, do something and reset here
        if(mi_code > 1){
            //reset data bytes;
            mi_data_byte1 = 0;
            mi_data_byte2 = 0;
        }
        
    }else{// if(mi_channel == writing_channel){
        mi_data_byte2 = mi_input_byte;

        if(mi_code == mi_note_on){
            mi_key = mi_data_byte1;
            
            if(mi_data_byte2 > 0){
                mi_midi_gate = true;
                set_current_note(mi_channel, mi_key, true);
            }else{
                mi_midi_gate = false;
                set_current_note(mi_channel, mi_key, false);
            }
        }else if(mi_code == mi_note_off){
            mi_midi_gate = false;
            mi_key = mi_data_byte1;
            set_current_note(mi_channel, mi_key, false);
        }else if(mi_code == controller_change
            && (mi_data_byte1 >=120 && mi_data_byte1 <= 127)
            ){
            //reset all
            reset_sequence();
        }

        //reset data bytes;
        mi_data_byte1 = 0;
        mi_data_byte2 = 0;
    }
  }else{

    unsigned long current_millis = millis();

    //                                             /2 <- (1023 / (total-1))
    rotary_value = floor((analogRead(rotary_pin) + 47) / 93);
    if(rotary_value != prev_rotary_value){
        editing_parameter = rotary_value;
        change_detected = true;
    }
    prev_rotary_value = rotary_value;

    step_value = floor((analogRead(step_pin) + 73) / 146);
    if(step_value != prev_step_value){
        editing_index = 7 - step_value;
        change_detected = true;
    }
    prev_step_value = step_value;

    pattern_value = floor((analogRead(pattern_pin) + 34) / 68);
    if(pattern_value != prev_pattern_value){
        editing_pattern = 15 - pattern_value;
        change_detected = true;
    }
    prev_pattern_value = pattern_value;

    toggle_state = !bitRead(PIND, 7);
    if(toggle_state
        && current_millis - last_toggled_pattern > 500){
        last_toggled_pattern = current_millis;
        play_pattern[editing_pattern] = !play_pattern[editing_pattern];
        change_detected = true;
    }
      
    bool clock_state = !bitRead(PIND, 4);
    if(clock_state
        && clock_state != prev_clock_state){
        last_encoder_read = current_millis;
        bool dir = bitRead(PIND, 5);
        bool sw_state = bitRead(PIND, 6);
        
        change_detected = true;
      
        if(editing_parameter == 0){
            // note
            pattern[editing_pattern][editing_index] += (dir?1:-1);
            pattern[editing_pattern][editing_index] = min(12,pattern[editing_pattern][editing_index]);
            pattern[editing_pattern][editing_index] = max(-12,pattern[editing_pattern][editing_index]);
        }else if(editing_parameter == 1){
            // note on/off
            if(pattern[editing_pattern][editing_index]>=255){
              pattern[editing_pattern][editing_index] = 0;
            }else if(pattern[editing_pattern][editing_index]<255){
              pattern[editing_pattern][editing_index] = 255;
            }
        }else if(editing_parameter == 2){
            // root octave
            root_note[editing_pattern] += (dir?12:-12);
            if(root_note[editing_pattern] < 0){
               root_note[editing_pattern] = 127;
            }else if(root_note[editing_pattern] > 127){
               root_note[editing_pattern] = 0;
            }
        }else if(editing_parameter == 3){
            root_note[editing_pattern] += (dir?1:-1);
            if(root_note[editing_pattern] < 0){
               root_note[editing_pattern] = 127;
            }else if(root_note[editing_pattern] > 127){
               root_note[editing_pattern] = 0;
            }
        }else if(editing_parameter == 4){
            // clock speed
            bpm+= (dir?1:-1);
            set_clock_interrupt();
        }else if(editing_parameter == 5){
            //resolution
            resolution[editing_pattern] += (dir?1:-1);
                  
            if(resolution[editing_pattern] > 128){
              resolution[editing_pattern] = 0;
            }else if(resolution[editing_pattern]<0){
              resolution[editing_pattern] = 128;
            }
            
            if(swing[editing_pattern] > resolution[editing_pattern]){
              swing[editing_pattern] = resolution[editing_pattern];
            }else if(swing[editing_pattern]<0){
              swing[editing_pattern] = 0;
            }
        }else if(editing_parameter == 6){
            //swing
            swing[editing_pattern]+= (dir?1:-1);
            if(swing[editing_pattern] > resolution[editing_pattern]){
              swing[editing_pattern] = resolution[editing_pattern];
            }else if(swing[editing_pattern]<0){
              swing[editing_pattern] = 0;
            }
        }else if(editing_parameter == 7){
            // scale
            scale += (dir?1:-1);
            if(scale < 0){
               scale = 6;
            }else if(scale > 6){
               scale = 0;
            }
        }else if(editing_parameter == 8){
            //key
            key += (dir?1:-1);
            if(key < 0){
               key = 11;
            }else if(key > 11){
               key = 0;
            }
        }else if(editing_parameter == 9){
            // midi channel
            channel[editing_pattern]+= (dir?1:-1);
            if(channel[editing_pattern]>15){
                channel[editing_pattern] = 0;
            }else if(channel[editing_pattern]<0){
                channel[editing_pattern] = 15;
            }
        }else if(editing_parameter == 10){
            // sequence length
            sequence_length[editing_pattern]+= (dir?1:-1);
            if(sequence_length[editing_pattern]>8){
                sequence_length[editing_pattern] = 0;
            }else if(sequence_length[editing_pattern]<0){
                sequence_length[editing_pattern] = 8;
            }
        }else if(editing_parameter == 11){
            // modify other pattern
            int new_value = modifier[editing_pattern] + (dir?1:-1);
            if(new_value > 2){
              play_pattern[editing_pattern] = false; // prevent unwanted changes
            }
          
            modifier[editing_pattern]+= (dir?1:-1);
            if(modifier[editing_pattern]>6){
                modifier[editing_pattern] = 0;
            }else if(modifier[editing_pattern]<0){
                modifier[editing_pattern] = 6;
            }
            for(int j=0; j<16; j++){
                if(modifier[j]!=0){
                    modified_by[channel[j]][j] = true;
                }else{
                    modified_by[channel[j]][j] = false;
                }
            }
        }
        
     }
      
     prev_clock_state = clock_state;
    
  }

  if(change_detected || clock_changed){
      
      u8g.firstPage();  
      do {
        draw();
      } while( u8g.nextPage() );
      
      change_detected = false;
      clock_changed = false;
  }
}

void draw(){

  int start_drawing = floor(editing_pattern / 2) * 2;

  for(int p=start_drawing; p<start_drawing+2; p++){
    byte d = p - start_drawing;
    for(int i=0; i<8; i++){
      
      if(i==sequence_length[p]-1){
        u8g.setFont(u8g_font_courB08);
      }else{
        u8g.setFont(u8g_font_profont10);
      }
      
      if(pattern[p][i]>=(255-8)){
        u8g.drawStr(circlex[d]+pointx[i],circley[d]+pointy[i], notetypenames[255-pattern[p][i]]);
      }else{
        itoa(pattern[p][i], buf, 10);
        u8g.drawStr(circlex[d]+pointx[i]+((pattern[p][i]<0)?-5:0),circley[d]+pointy[i],buf);
      }

      u8g.setFont(u8g_font_profont10);

      if(editing_pattern==p
          && editing_index==i){
        u8g.drawLine(circlex[d]+pointx[i], circley[d]+pointy[i]+2, circlex[d]+pointx[i]+5, circley[d]+pointy[i]+2 );
      }
    }
  
    u8g.drawLine(circlex[d], circley[d], poishx[sequence_index[p]] + circlex[d], poishy[sequence_index[p]] + circley[d]);

    sprintf(string_buffer,"%s%i",modifiernames[modifier[p]],channel[p]);
    u8g.drawStr(circlex[d]-5,circley[d],string_buffer);

    sprintf(string_buffer,"/%i",resolution[p]);
    u8g.drawStr(circlex[d]+15,circley[d]+30,string_buffer);

    sprintf(string_buffer,"s%i",swing[p]);
    u8g.drawStr(circlex[d]+15,circley[d]+23,string_buffer);

    if(midi_root_note[p]!=255){
      sprintf(string_buffer,"~%i",midi_root_note[p]);
    }else{
      sprintf(string_buffer,"~%i",root_note[p]);
    }
    u8g.drawStr(circlex[d]-20,circley[d]+30,string_buffer);

    u8g.setFont(u8g_font_courB08);
    itoa(p, buf, 10);
    u8g.drawStr(circlex[d],circley[d]+30,buf);
    u8g.setFont(u8g_font_profont10);
  }

  sprintf(string_buffer,"%ibpm",bpm);
  u8g.drawStr(48,7,string_buffer);

  u8g.drawStr(0, 7,keynames[key]);
  u8g.drawStr(10, 7,scalenames[scale]);

}

void clock_tick(){

  if(!bitRead(PINC, 0)){ // master switch
     for(int i=0; i<16; i++){
        if(previous_note[i]!=255){
          midi_out_note_off(previous_note[i], channel[i]);
          previous_note[i]=255;
        }
     }
     return;
  }

  clock_tick_counter++;

  bitWrite(PORTC, 5, (clock_tick_counter%2!=0)); // half rate clock out

  bool advanced_this_tick[16];

  //advance all sequence indexes, so that modifiers are all on the correct step
  for(int i=0; i<16; i++){
    advanced_this_tick[i] = false;
    if(!play_pattern[i]){
      if(previous_note[i]!=255){
        midi_out_note_off(previous_note[i], channel[i]);
        previous_note[i]=255;
      }
      continue;
    }
    
    if((beat_number[i]-swing[i]) % resolution[i] == 0){

       advanced_this_tick[i] = true;

       sequence_index[i] = (beat_number[i]-swing[i]) / resolution[i];

       if(d[i]){
         if(sequence_index[i] >= sequence_length[i]){
            sequence_index[i] = 0;
            beat_number[i] = swing[i];
         }
       }else{
         if(sequence_index[i] < 0){
            sequence_index[i] = sequence_length[i];
            beat_number[i] = beat_number[i] * resolution[i];
         }
       }
    }
    
    beat_number[i]+=(d[i]?1:-1);
  }

  for(int i=0; i<16; i++){
    
    if(advanced_this_tick[i]){

       if(previous_note[i]!=255
          && modifier[i]==0){
          midi_out_note_off(previous_note[i], channel[i]);
       }
  
       int note_from_pattern = pattern[i][sequence_index[i]];

       /* 
        "R", // 254 random
        "r", // 253 slightly random
        "V", // 252 very random
        "O", // 251 random octave change
        "^", // 250 toggle octave up
        "`", // 249 toggle octave down
        "<", // 248 toggle direction
        "!", // 247 stop pattern (so use as one-shot pattern, triggered by another pattern / button
       */
       if(pattern[i][sequence_index[i]]==254){
          note_from_pattern = random(-12, 13);
          random_offset[i] = note_from_pattern;
       }else if(pattern[i][sequence_index[i]]==253){
          note_from_pattern = random(-6, 6);
          random_offset[i] = note_from_pattern;
       }else if(pattern[i][sequence_index[i]]==252){
          note_from_pattern = random(-24, 25);
          random_offset[i] = note_from_pattern;
       }else if(pattern[i][sequence_index[i]]==251){
          octave_offset[i] = random(-2, 2);
          if(modifier[i]==1){
            octave_offset[channel[i]] = octave_offset[i];
          }else if(modifier[i]==2){
            octave_offset[channel[i]] = -octave_offset[i];
          }
       }else if(pattern[i][sequence_index[i]]==250){
          if(octave_offset[i]==1){
              octave_offset[i] = 0;
          }else{
              octave_offset[i] = 1;
          }
          if(modifier[i]==1){
            octave_offset[channel[i]] = octave_offset[i];
          }else if(modifier[i]==2){
            octave_offset[channel[i]] = -octave_offset[i];
          }
       }else if(pattern[i][sequence_index[i]]==249){
          if(octave_offset[i]==-1){
              octave_offset[i] = 0;
          }else{
              octave_offset[i] = -1;
          }
          if(modifier[i]==1){
            octave_offset[channel[i]] = octave_offset[i];
          }else if(modifier[i]==2){
            octave_offset[channel[i]] = -octave_offset[i];
          }
       }else if(pattern[i][sequence_index[i]]==248){
          d[i] = !d[i];
          if(!d[i]){
            beat_number[i]-=resolution[i];
          }
       }else if(pattern[i][sequence_index[i]]==247){
          play_pattern[i] = false;
       }

       if(pattern[i][sequence_index[i]]<=127 || (pattern[i][sequence_index[i]]>=252 && pattern[i][sequence_index[i]]<=254)){
           if(modifier[i]==3){ // override length of sequence
              int sanitised_sequence_length = abs(note_from_pattern);
              sanitised_sequence_length = min(8, sanitised_sequence_length);
              sequence_length[channel[i]] = sanitised_sequence_length;
           }else if(modifier[i]==4){ // override resolution
              int sanitised_resolution = abs(note_from_pattern);
              resolution[channel[i]] = sanitised_resolution;
           }else if(modifier[i]==5){ // toggle pattern on/off
              play_pattern[channel[i]] = !play_pattern[channel[i]];
           }else if(modifier[i]==6){ // swing
              swing[channel[i]] = abs(note_from_pattern);
              if(swing[channel[i]]>resolution[channel[i]]){
                 swing[channel[i]] = resolution[channel[i]];
              }
           }
       }

       int modified_root_note = root_note[i];
       if(midi_root_note[i]!=255){
          modified_root_note = midi_root_note[i];
       }

       for(int master=0; master<16; master++){
          if(modified_by[i][master]==true){
             if(modifier[master]==1){ // +
                if(pattern[master][sequence_index[master]]<=254 && pattern[master][sequence_index[master]]>=252){
                   modified_root_note+= random_offset[master];
                }else{
                   modified_root_note+= pattern[master][sequence_index[master]];
                }
             }else if(modifier[master]==2){ // -
                if(pattern[master][sequence_index[master]]<=254 && pattern[master][sequence_index[master]]>=252){
                   modified_root_note-= random_offset[master];
                }else{
                   modified_root_note-= pattern[master][sequence_index[master]];
                }
             }
          }
       }

       int note_index_within_scale = modified_root_note + note_from_pattern;
       int note_index = key + pgm_read_byte_near(&(scales[scale][note_index_within_scale])) + (octave_offset[i] * 12);
       
       if(note_index > 127){
          note_index = 127;
       }else if(note_index < 0){
          note_index = 0;
       }

       if((pattern[i][sequence_index[i]]<=127 || (pattern[i][sequence_index[i]]>=252 && pattern[i][sequence_index[i]]<=254))
          && modifier[i]==0){

          midi_out_note_on(note_index, 127, channel[i]);

          previous_note[i] = note_index;
       }else{
          previous_note[i] = 255;
       }
    }
    
  }

  clock_changed = true;
}

void sync(){

  for(int i=0; i<16; i++){
    
    beat_number[i] = 0;

  }

  clock_changed = true;
  
}

void midi_out_note_off(byte note_number, byte channel){
  byte status_byte = 128 | note_off | channel;
  Serial.write(status_byte);
  Serial.write(note_number);
  Serial.write(velocity_off);
}
void midi_out_note_on(byte note_number, byte velocity, byte channel){
  byte status_byte = 128 | note_on | channel;
  Serial.write(status_byte);
  Serial.write(note_number);
  Serial.write(velocity);
}

void set_current_note(byte channel, byte note, bool on){

  if(on){
    if(scale==0){
      midi_root_note[channel] = note;
    }else{
      byte found_note = 127;
      
      for(int i = 0; i<127; i++){
        byte lower_note = key + pgm_read_byte_near(&(scales[scale][i]));
        byte higher_note = key + pgm_read_byte_near(&(scales[scale][i+1]));

        if(note >= lower_note && note < higher_note){
          found_note = i;
          break;
        }
      }
      midi_root_note[channel] = found_note;
    }
  }else{
    midi_root_note[channel] = 255;
  }
}

void reset_sequence(){
  for(int i=0; i<16; i++){
    midi_root_note[i] = 255;
  }
}
