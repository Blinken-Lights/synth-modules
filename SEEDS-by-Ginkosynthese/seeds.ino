/*
"Seeds" Copyright (C) 2019-2020  Kassen Oud
Version 1.1

LICENSE:
This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 2 of the License, or
	(at your option) any later version.
	
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	
	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.

DESCRIPTION:
	Code for the Ginkosynthese "Seeds" module. A synthvoice/oscillator
	with a range of sounds. Please see the manual for details.

Change Log (newest first)
	V1.1
		Added a switch in the code to substitute your own modulator for the internal LFO
			for the pwm sounds (2 and 7);
		Added a 9th sound; an adaptation of the "West Coast Osc" code for the Grains module.
		More quantised mode fixing. (We had 1 more off-by-1 error than I thought...)
		Updated the manual to cover the new features.
		Clarified the relationship between the quantise mode and the "tune" knob in the manual.
		Various typos fixed, please excuse my dyslexia.
	V1.0
		Updated manual
		fixed bug where the mod&decay inputs stopped responding to signals while in shift mode
		Removed support for PCB version 1;
		Redid phase distortion sound (mode 5) to be way more interesting and efficient
		repeated checks for the gate through the main loop a few times as that's timing critical
		Fixed quantise mode again. Hopefully for real, this time. Sorry.
		Made wave-wrapper asymmetrical in mode 1 to preserve fundamental at all times
		fixed issues where quantise mode had 1 key too many, detuning everything
		sustain mode fixed
		Increased LFO range
		Reduced LFO depth
		use timer to measure time while holding shift
		fix jitter in quantise mode
		First attempt at scaling mod envelope with overall decay time
		display value while in shift mode
		Fixed issue with filter envelope not keeping up in rare cases
		Added square wave sound
		scale filter env max value by sustain level
		made max attack time longer
		Added manual section about updating the code
		Manual edits due to target market research, which was very important, professional and useful
		super-saw LFO control added
		removed sweep from hoover
		Added "philosophy" section to manual
		attack phase for mod env
		attack phase for filter env
		no more jumping values after going out of shift-mode
		only read knobs that have been moved a certain amount in shift mode
		store quantisation setting in EEPROM
		tuning quantisation option
		rough first draft of manual
		save filter & attack settings to EEPROM
		shift + decay = attack control
		shift + mod = filter control
		button "shift" function
		made extra analogue inputs for new PCB work
		redid noise sound
		found & fixed attack bug
		recalculated tuning table
		fixed 2nd order discontinuity in de phase dist. preset.
		fixed DC offset in the hardsync preset and anti-aliased the carrier
		Reduced resolution on tuned noise "mod" function to allow for more stable tones
		made volume env. fade to 127, instead of 0
		select button functionality
		split off the tuning stuff and led control to separate files
		selection knob auto scaling
		select sound at startup based on knob position
		remove traces of Little Synth keyboard stuff
		fixed bug in anti-aliassing of Hoover osc's 2&3
		started working on manual
		extended led control function to cover 16 values; 0-9,A-F
*/

//include files
#include <avr/io.h>	//device specific in/out mapping
#include <avr/interrupt.h> //as we need an interrupt-based DSP loop
#include <avr/pgmspace.h> //to store our frequency table in progmem
#include <EEPROM.h>	//permanently store settings under shift functions
#include "kas-button.h" //button de-bouncing
#include "led_ctrl.h" //control for 7segment led
#include "tuning.h" //pitch tracking, quantisation


//comment out this line (by putting "//" in front of it) to enable manual control over the PWM
//for sounds 2 and 7. The default is using an internal LFO with variable speed
#define INTERNAL_LFO


//definitions;

//amount of steps the knob needs to be moved in (or after) shift mode before we believe it was touched.
//should be as small as possible, but no smaller. This has to do with the quality of the pots and how
//much wear we anticipate
const uint8_t turn_treshold = 5;


// Map Analogue channels
#define FREQ_OFFSET				(0)			//Pitch input, marked 'CV1'. 
#define FREQ_MOD				(3)			//Input from the keyboard (used for both CV and gate).
#define SELECT_KNOB				(4)			//Sound selection knob
#define DECAY_CONTROL			(1)			//Decay knob.

#define MOD_CONTROL				(7)			//modulation knob
#define MOD_CV					(2)			//Mod CV in
#define MOD_DECAY_CV			(6)			//Decay CV in



//map Digital inputs;
#define KB_GATE					(10)		//Gate input
#define MODE_SWITCH				(19)		//mode switch button (A5)

// Changing these will also requires rewriting setup()
//    Output is on pin 11
#define PWM_PIN       11
#define PWM_VALUE     OCR2A
#define LED_PIN       13
#define LED_PORT      PORTB
#define LED_BIT       5
#define PWM_INTERRUPT TIMER2_OVF_vect

//constants


//table mapping the release knob's value to an amount to be subtracted from the volume envelope
const uint16_t release_map[16] = {1024, 256, 128, 112,  96, 82, 64, 48,  36, 28, 24, 20,  18, 16, 12, 8};
//table to set the attack_delta
const uint16_t attack_map[16] =  {2048, 1774, 1512, 756, 528,  310, 200, 138, 93, 47, 21, 16, 8, 4, 2, 1};

const uint16_t max_int = 65535;
const uint16_t half_max_int = 32767;
const uint16_t quarter_max_int = 16383;

//Buttons. These are used to debounce on/off inputs
Button mode_switch;
Button gate;

//variables relating to preset selection
uint8_t mode = 0;
uint8_t tmp_mode = 0;
uint8_t last_tmp_mode = 0;
const uint8_t grain_mode = 0;
const uint8_t tri_mode = 1;
const uint8_t saw_mode = 2;
const uint8_t fm_mode = 3;
const uint8_t hs_mode = 4;
const uint8_t pd_mode = 5;
const uint8_t noise_mode = 6;
const uint8_t square_mode = 7;
const uint8_t ring_mode = 8;
const uint8_t num_modes = 9;

//variables for the select/shift button
bool shift = false;
uint16_t shift_counter = 0;
bool button_down = false;

//Variables shared by all presets;
uint8_t mod_con_val = 0;
uint8_t dec_con_val = 0;
uint16_t master_phase = 0;
uint16_t m_phase_inc = 100;
uint16_t pwmv = 0;
uint16_t tmp_pwmv_sum = 0;
uint16_t quant_pwmv = 0;
uint16_t pwmv_hist_A = 0;
uint16_t pwmv_hist_B = 0;
bool gate_state = false;
bool attacking = false;
bool mod_attacking = false;
uint16_t attack_delta = 2047;
uint16_t decay = 1;
uint16_t env_value = 0;
uint16_t mod_env = 0;
uint16_t treated_mod_env = 0;
uint16_t mod_sustain = half_max_int;
uint16_t mod_decay = 4;
uint8_t decay_counter = 0;
uint16_t filter_env = 0;
uint16_t filt_decay = 4;
bool filt_attacking = false;
uint16_t filt_sustain = 90 * 255;
uint16_t filt_max = max_int;
uint16_t filt_val = 127;
uint16_t last_filt_val = 127;
uint32_t accumulator = 0;

//temporary values
uint8_t value = 0;
uint16_t output = 0;
uint16_t last_out = 0;
uint8_t mix_A = 0;
uint8_t mix_B = 0;
uint16_t out_A = 0;
uint16_t out_B = 0;
uint16_t pwmv_offset = 0;

//variables for permanent storage
const uint8_t init_byte_num = 0;
const uint8_t quant_byte_num = 1;
const uint8_t attack_byte_num = 2;
const uint8_t filt_byte_num = 3;
bool Need_to_write_quant = false;
bool Need_to_write_attack = false;
bool Need_to_write_filt = false;

bool quant_setting = 0;
bool last_quant_setting = 0;
uint8_t filt_setting = 0;
uint8_t attack_setting = 0;
uint8_t tmp_quant_setting = 0;
uint8_t tmp_attack_setting = 0;
uint8_t tmp_filt_setting = 0;
uint8_t init_quant_setting = 0;
uint8_t init_attack_setting = 0;
uint8_t init_filt_setting = 0;
uint8_t last_filt_write = 0;
uint8_t last_attack_write = 0;
bool direct_tune = true;
bool direct_decay = true;
bool direct_mod = true;

//mode 0 (grains) phases
//sound constants, edit these to tweak the sound
	const uint8_t grain_decay = 1;
	const uint8_t grain2_decay = 2;
uint16_t grainPhaseInc = 0;
uint16_t grainPhaseAcc = 0;
uint16_t grainAmp = 0;
uint16_t grain2PhaseAcc = 0;
uint16_t grain2PhaseInc = 0;
uint16_t grain2Amp = 0;
uint16_t grain2val = 0;

//mode 1 (wave-wrapper) variables
bool flip = false;
uint16_t wrap = 1;				// amount of wave-folding
uint16_t flip_wrap = 1;				// amount of wave-folding
uint16_t drive = 32;			//amount of clipping for positive signal
uint16_t top_drive = 32;		//same for negative signal

//mode 2 (super-saw) variables
bool cut = false;
bool last_cut1 = false;
bool last_cut2 = false;
bool last_cut3 = false;
uint16_t phase2 = 0;
uint16_t phase3 = 0;
uint16_t phase2_val = 0;
uint16_t phase3_val = 0;
uint32_t mod_value = 0;
#ifdef INTERNAL_LFO
uint8_t lfo_counter = 0;
uint16_t lfo_phase = 0;
uint16_t lfo_value = 0;
uint8_t lfo_phase_inc = 32;		//hoover & square wave sounds LFO speed
const uint8_t lfo_map[16] = {1, 2, 3, 4, 5, 6, 8, 12, 16, 24, 32, 48, 64, 82, 128, 255};
#else
uint16_t manual_mod_value = 0;
uint16_t pwm_treshold = 200;
#endif

//mode 3 (fm) variables
uint16_t carrier_phase = 0;
uint16_t carrier_value = 0;
uint16_t modulator_phase = 0;
uint16_t modulator_value = 0;
uint16_t fm_phase_inc = 1;
uint8_t fm_mod_depth = 100;

//mode 4 (hard sync) variables
uint16_t hs_mul = 1;
uint16_t last_carier = 0;

//mode 5 (phase distortion) variables
uint16_t treated_phase = 0;
uint8_t phase_dist_amount = 0;
uint16_t phase_mix_A = 0;
uint16_t phase_mix_B = 0;
uint16_t drive_factor = 0;
uint16_t o_drive = 255;
uint16_t d_mix_l = 0;
uint16_t d_mix_r = 0;

//mode 6 (noise) variables and function
uint8_t noise_count = 0;
uint8_t noise_length = 2;
uint8_t noise_reset_count = 0;
uint8_t last_noise_count = 0;
const uint8_t noise_map[8] = {1, 2, 3, 4, 6, 8, 16, 32};

//mode 7 (ring mod) variables
bool master_polarity = 0;
bool slave_polarity = 0;
uint16_t slave_offset_current = 1;
uint16_t slave_offset_measured = 1;
uint16_t wrap_factor = 64;
uint16_t neg_wrap_factor = 64;
uint16_t slave_phase = 0;
uint16_t master_value = 0;
uint16_t slave_value = 0;

//"xorshift" noise generator because "rand()" costs quite a bit of CPU
//borrowed from http://www.arklyffe.com/main/2010/08/29/xorshift-pseudorandom-number-generator/
//As cryptography and related algorithms are generally released under an MIT licence I believe that
//borrowing this for a GPL-licensed program should be fine.
uint16_t rand_seed = 1;
uint16_t set_seed = 1;

uint16_t rng(void)
{
	rand_seed ^= (rand_seed << 13);
	rand_seed ^= (rand_seed >> 9);
	return rand_seed ^= (rand_seed << 7);
}



//set up our Arduino at bootup
void setup() {
	if (EEPROM.read(init_byte_num) != 63) //picked 63 at random; not sure "empty" EEPROM is all 0
	{
		//initialise storage this should only happen when first started
		EEPROM.write(init_byte_num, 63);
		EEPROM.write(attack_byte_num, 2);
		EEPROM.write(filt_byte_num, 140);
	}
	filt_setting = EEPROM.read(filt_byte_num);
	filt_sustain = filt_setting * 255;
	filt_max = half_max_int + (filt_sustain >> 1);
	attack_setting = EEPROM.read(attack_byte_num);
	attack_delta = attack_map[ attack_setting ];
	quant_setting = EEPROM.read(quant_byte_num);
	
	// Set up PWM to 31.25kHz, phase accurate
	TCCR2A = _BV(COM2A1) | _BV(WGM20);
	TCCR2B = _BV(CS20);
	TIMSK2 = _BV(TOIE2);
	// configure the pins we use
	pinMode(PWM_PIN,OUTPUT);
	pinMode(LED_PIN,OUTPUT);
	setupLedPins();
	pinMode(KB_GATE, INPUT_PULLUP);
	pinMode(MODE_SWITCH, INPUT_PULLUP);
	//set up our button debouncing 
	gate.pin = KB_GATE;
	mode_switch.pin = MODE_SWITCH;
	tmp_mode = ((analogRead(SELECT_KNOB) >> 2) * num_modes ) >> 8;
	last_tmp_mode = tmp_mode;
	setMode(tmp_mode);
	writeNum(mode);
}


//simple lowpass filter using two one-pole filters in series
uint16_t filter( uint16_t signal, uint16_t cutoff )
{
	cutoff *= cutoff;
	cutoff >>= 8;
	mix_B = max(1, cutoff);			//say "no" to DC offset
	mix_A = 255 - mix_B;
	out_A = (out_A * mix_A) + (signal * mix_B);
	out_A = out_A >> 8;
	out_B = (out_B * mix_A) + (out_A * mix_B);
	out_B = out_B >> 8;
	return out_B;
}

void setMode(uint8_t m)
{
	if (m >= num_modes) mode = 0;
	else mode = m;
}

uint8_t diff(uint8_t A, uint8_t B)
{
	return (A > B)?A-B:B-A;
}

void gateFun()
{
	//note-on detected
	if (gate.state) 
	{
		LED_PORT |= 1 << LED_BIT;
		gate_state = true;
		attacking = true;
		filt_attacking = true;
		mod_attacking = true;
		//filt_sustain = (90 + (pwmv >> 4)) * 255;
		if (mode == saw_mode)
		{
			master_phase = 0;
			phase2 = 0;
			phase3 = 0;
		}
	}
	//note off
	else
	{
		LED_PORT ^= 1 << LED_BIT;
		gate_state = false;
	}
}


//main loop. This deals with getting and processing our various inputs
void loop()
{
	if( updateButton( &gate)) //input debouncing
	{
		gateFun();
	}

	pwmv = analogRead(FREQ_MOD);
	if (shift)
	{
		tmp_quant_setting = analogRead(FREQ_OFFSET) >> 2;
		if (diff (tmp_quant_setting, init_quant_setting) > turn_treshold)
		{
			quant_setting =  (tmp_quant_setting > 127)?true:false;
			if (quant_setting != last_quant_setting)
			{
				writeSide(quant_setting);
				Need_to_write_quant = true;
				last_quant_setting = quant_setting;
			}
		}
	}
	else if (!direct_tune)
	{
		if (diff( init_quant_setting, (uint16_t)analogRead(FREQ_OFFSET) >> 2) > turn_treshold) direct_tune = true;
	}
	else
	{
		pwmv_offset =  analogRead(FREQ_OFFSET) >> 2;
		pwmv_offset *= 205;
		pwmv_offset = pwmv_offset >> 8;
	}
	if (quant_setting)
	{
		//true if there is either a large change or the value is consistent
		//we aim to prevent jitter between two notes when the read value is
		//close to the threshold
		if(diff( pwmv, pwmv_hist_A) > turn_treshold ||
			(pwmv == pwmv_hist_A && pwmv == pwmv_hist_B))
		{
			quant_pwmv = mapQuant( pwmv );
		}
		m_phase_inc = mapFreq( min( 1023, quant_pwmv + pwmv_offset) );
		pwmv_hist_B = pwmv_hist_A;
		pwmv_hist_A = pwmv;
	}
	else
	{
		m_phase_inc = mapFreq( min( 1023, pwmv + pwmv_offset) );
	}

	if( updateButton( &gate)) //input debouncing
	{
		gateFun();
	}

	if (!shift)
	{
		tmp_mode = ((analogRead(SELECT_KNOB) >> 2) * num_modes ) >> 8;
		if (tmp_mode != last_tmp_mode)
		{
			writeNum(tmp_mode);
			last_tmp_mode = tmp_mode;
			if (tmp_mode != mode) digitalWrite(LED_DOT, 1);
			else digitalWrite(LED_DOT, 0);
		}
	}

	if ( updateButton ( &mode_switch))
	{
		if (mode_switch.state)
		{
			button_down = true;
			shift_counter = 0;
		}
		else						//negative edge
		{
			button_down = false;
			if (!shift)
			{
				setMode(tmp_mode);
				digitalWrite(LED_DOT, 0);
			}
			else
			{
				shift = false;
				if (Need_to_write_filt)
				{
					EEPROM.write(filt_byte_num, filt_setting);
					Need_to_write_filt = false;
					direct_mod = false;
					init_filt_setting = (uint16_t)analogRead(MOD_CONTROL) >> 2;
				}
				if (Need_to_write_attack)
				{
					EEPROM.write( attack_byte_num, attack_setting );
					Need_to_write_attack = false;
					direct_decay = false;
					init_attack_setting = (uint16_t)analogRead(DECAY_CONTROL) >> 2;
				}
				if (Need_to_write_quant)
				{
					EEPROM.write( quant_byte_num, quant_setting );
					Need_to_write_quant = false;
					direct_tune = false;
					init_quant_setting = (uint16_t)analogRead(FREQ_OFFSET) >> 2;
				}
				writeNum(tmp_mode);
				if (tmp_mode != mode) digitalWrite(LED_DOT, 1);
				else digitalWrite(LED_DOT, 0);
			}
		}
	}

	if (button_down & !shift)
	{
		if (shift_counter > half_max_int)
		{
			shift = true;
			writeShift();
			tmp_quant_setting = ((uint16_t)analogRead(FREQ_OFFSET) >> 2);
			init_quant_setting = tmp_quant_setting;
			tmp_attack_setting = ((uint16_t)analogRead(DECAY_CONTROL) >> 2);
			init_attack_setting = tmp_attack_setting;
			tmp_filt_setting = ((uint16_t)analogRead(MOD_CONTROL) >> 2);
			init_filt_setting = tmp_filt_setting;
		}
	}

	if (shift)
	{
		tmp_filt_setting = analogRead(MOD_CONTROL) >> 2;
		if (diff( tmp_filt_setting, init_filt_setting) > turn_treshold)
		{
			if (diff(tmp_filt_setting, last_filt_write) > turn_treshold)
			{
				writeValueDisp(tmp_filt_setting);
				last_filt_write = tmp_filt_setting;
			}
			filt_setting = tmp_filt_setting;
			filt_sustain = filt_setting << 8;
			filt_max = half_max_int + (filt_sustain >> 1);
			if (gate_state && !attacking && !(filter_env > filt_sustain) ) filter_env = filt_sustain;
			Need_to_write_filt = true;
		}
	}
	else if (!direct_mod)
	{
		if (diff( init_filt_setting, (uint16_t)analogRead(MOD_CONTROL) >> 2) > turn_treshold) direct_mod = true;
	}
	else
	{
		mod_con_val = analogRead(MOD_CONTROL) >> 2;
	}
	switch (mode)
	{
		case grain_mode:
			grain2PhaseInc = mapFreq( (mod_con_val * ((uint16_t)analogRead(MOD_CV) >> 2)) >> 6 ) << 2;
			//avoid fetching progmem in the DSP loop
			grainPhaseInc = max( 600, mapFreq(mod_env >> 6) * 2 );
			break;
		case tri_mode:
			wrap = min( 64 + (mod_con_val * ((uint16_t)analogRead(MOD_CV) >> 2) >> 7), 511);
			flip_wrap = max(64, wrap >>1);
			drive = min( 32 + (mod_env >> 7), 511);
			top_drive = max( 32, drive >>1);
			break;
		case saw_mode:
#ifdef INTERNAL_LFO
			lfo_phase_inc = lfo_map[(mod_con_val * (((uint16_t)analogRead(MOD_CV) >> 2) >> 4)) >> 8];
#else
			manual_mod_value = mod_con_val * ((uint16_t)analogRead(MOD_CV) >> 2);
#endif
			break;
		case fm_mode:
			fm_phase_inc = m_phase_inc;
			accumulator = m_phase_inc * ((mod_con_val * ((uint16_t)analogRead(MOD_CV) >> 2)) >> 8 );
			accumulator >>= 8;
			fm_phase_inc += accumulator;
			break;
		case hs_mode:
			hs_mul = (mod_con_val * ((uint16_t)analogRead(MOD_CV) >> 2)) >> 6;
			break;
		case pd_mode:
			phase_dist_amount = (mod_con_val * ((uint16_t)analogRead(MOD_CV) >> 2)) >> 8;
			break;
		case noise_mode:
			noise_length = noise_map[ (mod_con_val * ((uint16_t)analogRead(MOD_CV) >> 2)) >> 13];
			break;
		case square_mode:
#ifdef INTERNAL_LFO
			lfo_phase_inc = lfo_map[(mod_con_val * (((uint16_t)analogRead(MOD_CV) >> 2) >> 4)) >> 8];
#else
			manual_mod_value = mod_con_val * ((uint16_t)analogRead(MOD_CV) >> 2);
#endif
			break;
		case ring_mode:
			wrap_factor = min( 64 + ((mod_con_val * ((uint16_t)analogRead(MOD_CV) >> 2)) >> 7), 511);
			neg_wrap_factor = max(64, wrap_factor >>1);
			break;
	}

	if( updateButton( &gate)) //input debouncing
	{
		gateFun();
	}

	if (shift)
	{
		tmp_attack_setting = analogRead(DECAY_CONTROL) >> 2;
		if (diff (tmp_attack_setting, init_attack_setting) > turn_treshold)
		{
			if (diff( tmp_attack_setting, last_attack_write) > turn_treshold)
			{
				writeValueDisp(tmp_attack_setting);
				last_attack_write = tmp_attack_setting;
			}
			attack_setting =  tmp_attack_setting >> 4;
			attack_delta = attack_map[ attack_setting ];
			Need_to_write_attack = true;
		}
	}
	else if (!direct_decay)
	{
		if (diff( init_attack_setting, (uint16_t)analogRead(DECAY_CONTROL) >> 2) > turn_treshold) direct_decay = true;
	}
	else
	{
		dec_con_val = analogRead(DECAY_CONTROL) >> 2;
	}
		decay = release_map[(dec_con_val * ((uint16_t)analogRead(MOD_DECAY_CV) >> 2)) >> 12];
		mod_decay = max(decay >> 1, mode==grain_mode?127:4);
}


void grainProcess()
{
	if (master_phase < m_phase_inc) {
		// Time to start the next grain
		grainPhaseAcc = 0;
		grain2PhaseAcc = 0;
		grainAmp = 0x7fff;
		grain2Amp = 0x7fff;
	}

	// Increment the phase of the grain oscillators
	grainPhaseAcc += grainPhaseInc;
	grain2PhaseAcc += grain2PhaseInc;

	// Convert phase into a triangle wave
	value = (grainPhaseAcc >> 7) & 0xff;
	if (grainPhaseAcc & 0x8000) value = ~value;
	// Multiply by current grain amplitude to get sample
	output = value * (grainAmp >> 8);
	output += ((0xfff - grainAmp) >> 8) >> 1; //get rid of DC offset

	// Repeat for second grain
	value = (grain2PhaseAcc >> 7) & 0xff;
	if (grain2PhaseAcc & 0x8000) value = ~value;
	grain2val = value * (grain2Amp >> 8);
	grain2val += ((0xfff - grain2Amp) >> 8) >> 1; //get rid of DC offset

	if ((max_int - output) < grain2val) output = max_int; //clipping before the int overflows
	else output += grain2val;

	// Make the grain amplitudes decay by a factor every sample (exponential decay)
	grainAmp -= (grainAmp >> 8) * grain_decay;
	grain2Amp -= (grain2Amp >> 8) * grain2_decay;
	output >>= 8;
}


void triProcess()
{
	flip = master_phase & 0b1000000000000000;
	//turn the phase acumulator into 4 up-ramps
	output = (master_phase &  0b0011111111111111);
	//get these into 7 bit range. "flip" will be the 8th bit of the output
	//output = output >> 7;
	output = output << 1;
	output = output >> 8;
	//invert the 2nd and 4th of our 4 up-ramps to create our triangle wave
	if (master_phase & 0b0100000000000000) output = (uint16_t)127 - output;

	//amplify the signal for the wave-wrapping
	output *= flip?wrap:flip_wrap;
	//output = output >> 6;
	output = output << 2;
	output = output >> 8;
	//detect whether folding will be more involved than inverting the range
	//from 128 to 254
	if (output & 0b1111111100000000) 
	{
		//values between 255 and 511 fold back past the "0" line
		if (output & 0b0000000100000000) flip = !flip;
		//mask out bits beyond which the process just repeats
		output &= 0b0000000011111111;
	}
	//actual folding
	if (output > (uint16_t)127)
	{
		output = 127 - (output & 0b0000000001111111);
	}

	//apply the signal gain for asymmetrical clipping
	output *= flip?drive:top_drive;
	output = output >> 5;
	//clip
	if (output >  (uint16_t)127) output = (uint16_t)127;

	//turn our 7bit unipolar value into a 8bit bipolar one
	if ( flip ) output = (uint16_t)127 - output;
	else output += (uint16_t)127;
}

void superSawProcess()
{
#ifdef INTERNAL_LFO
	lfo_counter++;
	if (! (lfo_counter & 0b00000111)) lfo_phase += lfo_phase_inc;
	if (lfo_phase & 0b1000000000000000) lfo_value = ~lfo_phase;
	else lfo_value = lfo_phase;
	lfo_value >>= 8;
	lfo_value *= 200;
	if ((master_phase & 0b0111111111111111) < lfo_value) cut = true;
#else
	pwm_treshold = manual_mod_value;
	pwm_treshold >>= 1;
	if ((master_phase & 0b0111111111111111) < pwm_treshold) cut = true;
#endif
	else cut = false;
	if (master_phase < m_phase_inc)
	{
		output = 32768;
	}
	else if (cut != last_cut1)
	{
		output = master_phase >> 1;
		last_cut1 = cut;
	}
	else if (cut)
	{
		output = (uint16_t)0;
	}
	else
	{
		output = master_phase;
	}
	output = output >> 7;
	//phase2
	phase2 += (m_phase_inc + 3);
#ifdef INTERNAL_LFO
	if ((phase2 & 0b0111111111111111) < lfo_value) cut = true;
#else
	if ((phase2 & 0b0111111111111111) < pwm_treshold) cut = true;
#endif
	else cut = false;
	if (phase2 < (m_phase_inc + 3))
	{
		phase2_val = 32768;
	}
	else if (cut != last_cut2)
	{
		phase2_val = phase2_val >> 1;
		last_cut2 = cut;
	}
	else if (cut)
	{
		phase2_val = (uint16_t)0;
	}
	else
	{
		phase2_val = phase2;
	}
	//phase3
	phase3 += (m_phase_inc - 2);
#ifdef INTERNAL_LFO
	if ((phase3 & 0b0111111111111111) < lfo_value) cut = true;
#else
	if ((phase3 & 0b0111111111111111) < pwm_treshold) cut = true;
#endif
	else cut = false;
	if (phase3 < (m_phase_inc - 2))
	{
		phase3_val = 32768;
	}
	else if (cut != last_cut3)
	{
		phase3_val = phase2_val >> 1;
		last_cut3 = cut;
	}
	else if (cut)
	{
		phase3_val = (uint16_t)0;
	}
	else
	{
		phase3_val = phase3;
	}
	output += (phase2_val >>8);
	output += (phase3_val >>8);
	output >>= 2;
}


//Simple FM synth based on approximating two sines, with one modulating the phase of the other.
//Nearly all fm implementations are actually phase modulation, with good reason.
void fmProcess()
{
	fm_mod_depth = max( 25, mod_env >> 8);
	carrier_phase = master_phase;
	modulator_phase += fm_phase_inc;
	//We turn an 16 bit up-ramp into 2 8 bit up-ramps
	modulator_value = modulator_phase;
	modulator_value <<= 1;
	modulator_value >>= 8;
	//multiply by the inverse to approximate 2 half sines
	modulator_value *= (~modulator_value) & 255;
	modulator_value >>= 6;
	modulator_value *= fm_mod_depth;
	//no need to reconstruct the half sines into a sine, we can just use the first half for
	//positive modulation, then subtract the 2nd half.
	if (modulator_phase & 0b1000000000000000) carrier_phase += modulator_value;
	else carrier_phase -= modulator_value;
	//repeat the same for the carrier
	carrier_value = carrier_phase;
	carrier_value <<= 1;
	carrier_value >>= 8;
	carrier_value *= (~carrier_value) & 255;
	//down to 7 bits; the last bit is for the sign
	carrier_value >>= 7;
	//this time we reconstruct the whole sine
	if (carrier_phase & 0b1000000000000000) output = 127 - carrier_value;
	else output = 127 + carrier_value;
}


//this strategy for implementing hard-sync is based on my conversations with Rob Hordijk
//it's fast, and when we overshoot the master phase's zero-crossing then that doesn't cause
//jitter in the carrier, which is a major advantage.
//Basically we multiply the master phase by some factor and let that overflow (here we fake the
//overflow as we need the resolution). This gives us a new saw wave with a frequency above the 
//master phase. Then we apply an inverse of the master phase ramp as a volume contour, to get
//rid is the discontinuity at the sync point
void hardSyncProsess()
{
	//fake 32 bit accumulator as we need the resolution
	accumulator = master_phase;
	//note that hs_mul is 10 bits, so this goes up by 2 bits, or a factor of 4, being 2 octaves.
	accumulator *= hs_mul;
	accumulator >>= 8;
	//going up an octave for the carrier is arbitrary, but if we don't then we lose a lot of
	//volume for lower settings of the modulation knob, so we just do it; as I see it this little
	//synth is about having fun.
	//accumulator += master_phase;
	accumulator += master_phase;
	accumulator += master_phase;
	//there's the fake overflow I mentioned.
	accumulator &= max_int;
	//multiply by the inverse of the phase
	accumulator *= ~master_phase;
	accumulator >>= 16;
	//we're back into 16 bits, then down to the final 8 bit.
	output = accumulator;
	output >>= 8;
	//antialiassing
	if (output < last_carier)
	{
		output = last_carier >> 1;
		last_carier = 0;
	}
	else last_carier = output;
	//deal with DC Offset
	output += (master_phase >> 8) >> 1;
}

void phaseDistortionProcess()
{
	treated_phase = master_phase;
	treated_phase = (treated_phase >> 8) * (~treated_phase >> 8);
	treated_phase  = (treated_phase >> 6) * (phase_dist_amount << 2);
	treated_phase += master_phase;
	output = treated_phase;
	output <<= 1;
	output >>= 8;
	output *= (~output) & 255;
	output >>= 8;
	output <<= 1;
	if (treated_phase & 0b1000000000000000) output += 127;
	else output = 127 - output;
}


void noiseProcess()
{
	if (master_phase < m_phase_inc) noise_reset_count++;
	if (noise_reset_count >= noise_length)
	{
		rand_seed = set_seed;
		noise_reset_count = 0;
	}
	noise_count = master_phase >> 8;
	noise_count &= 0b11111000;
	if (noise_count != last_noise_count || true)
	{
		output = rng() >> 8;
		last_noise_count = noise_count;
	}
}

void squareProcess()
{
#ifdef INTERNAL_LFO
	lfo_counter++;
	if (! (lfo_counter & 0b00000111)) lfo_phase += lfo_phase_inc;
	if (lfo_phase & 0b1000000000000000) lfo_value = ~lfo_phase;
	else lfo_value = lfo_phase;
	lfo_value += quarter_max_int;

	if (master_phase  < lfo_value) cut = true;
#else
	pwm_treshold = manual_mod_value;
	pwm_treshold >>= 1;
	pwm_treshold += half_max_int;
	if (master_phase  < pwm_treshold) cut = true;
#endif
	else cut = false;
	if (cut != last_cut1)
	{
		output = 127;
		last_cut1 = cut;
	}
	else if (cut)
	{
		//we don't go from 0 to 255 as that'd make the square way louder than the other sounds
		output = (uint16_t)50;
	}
	else
	{
		output = (uint16_t)205;
	}
}

void ringProcess()
{
	/*
	slave_offset_measured = mod_env;
	slave_offset_measured >>= 6;
	//slave osc pitch offset smoothing. Without this we'd get undesired clicks in the sound
	if (slave_offset_current < slave_offset_measured) slave_offset_current++;
	else if (slave_offset_current > slave_offset_measured) slave_offset_current--;
	*/
	slave_offset_current = mod_env >> 6;

	//turn master into a sine, or rather; a close approximation. The proper sine function
	//is slow and practical tables are lo-fi and/or slow as progmem is so slow
	master_value = master_phase;
	master_value <<= 1;
	master_value >>= 8;
	master_value *= (~master_value) & 255;
	master_value <<= 2;
	master_value >>= 8;
	master_polarity = master_phase & 0b1000000000000000;

	//calculate the slave phase in 32bit resolution
	//multiply the phase by a 10 bit number, then go down 8 bits again
	//the result will be a 2 octave increase max.
	//On top of that we say that we want the slave to at least run at
	//the master's phase's rate.
	//Finally we truncate back into 16 bit, as though the slave phase flowed over
	accumulator = master_phase;
	accumulator *= slave_offset_current;
	accumulator >>= 8;
	accumulator += master_phase;
	accumulator &= max_int;

	slave_phase = accumulator;

	//turn slave into a sine too, like we did with the master.
	slave_value = slave_phase;
	slave_value <<= 1;
	slave_value >>= 8;
	slave_value *= (~slave_value) & 255;
	slave_value <<= 2;
	slave_value >>= 8;
	slave_polarity = slave_phase & 0b1000000000000000;

	//multiply master and slave for the ring-mod,
	//taking care to invert the phase where needed
	master_value *= slave_value;
	master_value >>= 8;
	master_polarity ^= slave_polarity;

	master_value = master_value >> 1;

	//wave-wrapping can result in frequency doubling, and we want to preserve bass
	//so we use asymmetrical wave-wrapping
	master_value *= master_polarity?wrap_factor:neg_wrap_factor;

	//bit-shifting by 8 bits is way faster than by other amounts
	//so this is faster than >>= 6, as we know the topmost bits are 0
	master_value <<= 2;
	master_value >>= 8;

	//detect whether folding will be more involved than inverting the range
	//from 128 to 254
	if (master_value & 0b1111111100000000) 
	{
		//values between 255 and 511 fold back past the "0" line
		if (master_value & 0b0000000100000000) master_polarity = !master_polarity;
		//mask out bits beyond which the process just repeats
		master_value &= 0b0000000011111111;
	}
	//actual folding
	if (master_value > (uint16_t)127)
	{
		master_value = 127 - (master_value & 0b0000000001111111);
	}

	//only now do we make a bipolar signal
	if (master_polarity) output = 127 - master_value;
	else output = 128 + master_value;
}

SIGNAL(PWM_INTERRUPT)
{
	output = 0;
	master_phase += m_phase_inc;
	decay_counter++;
	decay_counter &= 0b00000111;

	switch (mode)
	{
		case grain_mode:
			grainProcess();
			break;
		case tri_mode:
			triProcess();
			break;
		case saw_mode:
			superSawProcess();
			break;
		case fm_mode:
			fmProcess();
			break;
		case hs_mode:
			hardSyncProsess();
			break;
		case pd_mode:
			phaseDistortionProcess();
			break;
		case noise_mode:
			noiseProcess();
			break;
		case square_mode:
			squareProcess();
			break;
		case ring_mode:
			ringProcess();
			break;
	}


	//enveloppe stuff
	if (attacking)
	{
		if (env_value <= (max_int - attack_delta))
		{
			env_value += attack_delta;
		}
		else
		{
			env_value = max_int;
			attacking = false;
		}
	}
	else if (gate_state)
	{
		if (!filt_attacking)
		{
			if (filter_env  > filt_sustain + filt_decay) filter_env -= filt_decay;
			else filter_env = filt_sustain;
			filt_val = filter_env >> 8;
		}
	}
	else
	{
		if (!decay_counter)
		{
			if (env_value > decay) env_value -= decay;
			else env_value = 0;
		}
		if (!filt_attacking) filt_val = min(env_value, filter_env) >> 8;
	}
	if (mod_attacking)
	{
		if (mod_env < max_int - attack_delta) mod_env += attack_delta;
		else
		{
			mod_env = max_int;
			mod_attacking = false;
		}
	}
	else if ( gate_state)
	{
		if (mod_env > mod_decay + mod_sustain) mod_env -= mod_decay;
		else mod_env = mod_sustain;
	}
	else if (mod_env > mod_decay) mod_env -= mod_decay;
	else mod_env = 0;
	if (filt_attacking)
	{
		if (filter_env <= filt_max - (attack_delta << 1)) filter_env += (attack_delta << 1);
		else
		{
			mod_env = filt_max;
			filt_attacking = false;
		}
		filt_val = filter_env >> 8;
	}

	//apply filter
	output = filter (output, filt_val );

	//apply envelope
	output *= (env_value >> 8);
	output >>= 8;
	output += (~env_value >> 8) >> 1;

	// Output to PWM (this is faster than using analogWrite)  
	PWM_VALUE = output;
	if (button_down & !shift) shift_counter++;
}
