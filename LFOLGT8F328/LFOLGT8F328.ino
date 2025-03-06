byte analoguePins[] = { A7, A6, A5, A4, A3, A2, A1 };
int analogueReadings[] = { -99, -99, -99, -99, -99, -99, -99 };
byte analogueReadingsByte[] = { 0, 0, 0, 0, 0, 0, 0 };
int jitterThresholds[] = { 14, 14, 14, 14, 50, 14, 14 };
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

int amplitude = 255;

int phase = 0;

const byte waves[][1024] PROGMEM = {
  { 255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0 },
  { 0,	0,	0,	1,	1,	1,	1,	2,	2,	2,	2,	3,	3,	3,	3,	4,	4,	4,	4,	5,	5,	5,	5,	6,	6,	6,	6,	7,	7,	7,	7,	8,	8,	8,	8,	9,	9,	9,	9,	10,	10,	10,	10,	11,	11,	11,	11,	12,	12,	12,	12,	13,	13,	13,	13,	14,	14,	14,	14,	15,	15,	15,	15,	16,	16,	16,	16,	17,	17,	17,	17,	18,	18,	18,	18,	19,	19,	19,	19,	20,	20,	20,	20,	21,	21,	21,	21,	22,	22,	22,	22,	23,	23,	23,	23,	24,	24,	24,	24,	25,	25,	25,	25,	26,	26,	26,	26,	27,	27,	27,	27,	28,	28,	28,	28,	29,	29,	29,	29,	30,	30,	30,	30,	31,	31,	31,	31,	32,	32,	32,	32,	33,	33,	33,	33,	34,	34,	34,	34,	35,	35,	35,	35,	36,	36,	36,	36,	37,	37,	37,	37,	38,	38,	38,	38,	39,	39,	39,	39,	40,	40,	40,	40,	41,	41,	41,	41,	42,	42,	42,	42,	43,	43,	43,	43,	44,	44,	44,	44,	45,	45,	45,	45,	46,	46,	46,	46,	47,	47,	47,	47,	48,	48,	48,	48,	49,	49,	49,	49,	50,	50,	50,	50,	51,	51,	51,	51,	52,	52,	52,	52,	53,	53,	53,	53,	54,	54,	54,	54,	55,	55,	55,	55,	56,	56,	56,	56,	57,	57,	57,	57,	58,	58,	58,	58,	59,	59,	59,	59,	60,	60,	60,	60,	61,	61,	61,	61,	62,	62,	62,	62,	63,	63,	63,	63,	64,	64,	64,	64,	64,	65,	65,	65,	65,	66,	66,	66,	66,	67,	67,	67,	67,	68,	68,	68,	68,	69,	69,	69,	69,	70,	70,	70,	70,	71,	71,	71,	71,	72,	72,	72,	72,	73,	73,	73,	73,	74,	74,	74,	74,	75,	75,	75,	75,	76,	76,	76,	76,	77,	77,	77,	77,	78,	78,	78,	78,	79,	79,	79,	79,	80,	80,	80,	80,	81,	81,	81,	81,	82,	82,	82,	82,	83,	83,	83,	83,	84,	84,	84,	84,	85,	85,	85,	85,	86,	86,	86,	86,	87,	87,	87,	87,	88,	88,	88,	88,	89,	89,	89,	89,	90,	90,	90,	90,	91,	91,	91,	91,	92,	92,	92,	92,	93,	93,	93,	93,	94,	94,	94,	94,	95,	95,	95,	95,	96,	96,	96,	96,	97,	97,	97,	97,	98,	98,	98,	98,	99,	99,	99,	99,	100,	100,	100,	100,	101,	101,	101,	101,	102,	102,	102,	102,	103,	103,	103,	103,	104,	104,	104,	104,	105,	105,	105,	105,	106,	106,	106,	106,	107,	107,	107,	107,	108,	108,	108,	108,	109,	109,	109,	109,	110,	110,	110,	110,	111,	111,	111,	111,	112,	112,	112,	112,	113,	113,	113,	113,	114,	114,	114,	114,	115,	115,	115,	115,	116,	116,	116,	116,	117,	117,	117,	117,	118,	118,	118,	118,	119,	119,	119,	119,	120,	120,	120,	120,	121,	121,	121,	121,	122,	122,	122,	122,	123,	123,	123,	123,	124,	124,	124,	124,	125,	125,	125,	125,	126,	126,	126,	126,	127,	127,	127,	127,	128,	128,	128,	128,	128,	129,	129,	129,	129,	130,	130,	130,	130,	131,	131,	131,	131,	132,	132,	132,	132,	133,	133,	133,	133,	134,	134,	134,	134,	135,	135,	135,	135,	136,	136,	136,	136,	137,	137,	137,	137,	138,	138,	138,	138,	139,	139,	139,	139,	140,	140,	140,	140,	141,	141,	141,	141,	142,	142,	142,	142,	143,	143,	143,	143,	144,	144,	144,	144,	145,	145,	145,	145,	146,	146,	146,	146,	147,	147,	147,	147,	148,	148,	148,	148,	149,	149,	149,	149,	150,	150,	150,	150,	151,	151,	151,	151,	152,	152,	152,	152,	153,	153,	153,	153,	154,	154,	154,	154,	155,	155,	155,	155,	156,	156,	156,	156,	157,	157,	157,	157,	158,	158,	158,	158,	159,	159,	159,	159,	160,	160,	160,	160,	161,	161,	161,	161,	162,	162,	162,	162,	163,	163,	163,	163,	164,	164,	164,	164,	165,	165,	165,	165,	166,	166,	166,	166,	167,	167,	167,	167,	168,	168,	168,	168,	169,	169,	169,	169,	170,	170,	170,	170,	171,	171,	171,	171,	172,	172,	172,	172,	173,	173,	173,	173,	174,	174,	174,	174,	175,	175,	175,	175,	176,	176,	176,	176,	177,	177,	177,	177,	178,	178,	178,	178,	179,	179,	179,	179,	180,	180,	180,	180,	181,	181,	181,	181,	182,	182,	182,	182,	183,	183,	183,	183,	184,	184,	184,	184,	185,	185,	185,	185,	186,	186,	186,	186,	187,	187,	187,	187,	188,	188,	188,	188,	189,	189,	189,	189,	190,	190,	190,	190,	191,	191,	191,	191,	191,	192,	192,	192,	192,	193,	193,	193,	193,	194,	194,	194,	194,	195,	195,	195,	195,	196,	196,	196,	196,	197,	197,	197,	197,	198,	198,	198,	198,	199,	199,	199,	199,	200,	200,	200,	200,	201,	201,	201,	201,	202,	202,	202,	202,	203,	203,	203,	203,	204,	204,	204,	204,	205,	205,	205,	205,	206,	206,	206,	206,	207,	207,	207,	207,	208,	208,	208,	208,	209,	209,	209,	209,	210,	210,	210,	210,	211,	211,	211,	211,	212,	212,	212,	212,	213,	213,	213,	213,	214,	214,	214,	214,	215,	215,	215,	215,	216,	216,	216,	216,	217,	217,	217,	217,	218,	218,	218,	218,	219,	219,	219,	219,	220,	220,	220,	220,	221,	221,	221,	221,	222,	222,	222,	222,	223,	223,	223,	223,	224,	224,	224,	224,	225,	225,	225,	225,	226,	226,	226,	226,	227,	227,	227,	227,	228,	228,	228,	228,	229,	229,	229,	229,	230,	230,	230,	230,	231,	231,	231,	231,	232,	232,	232,	232,	233,	233,	233,	233,	234,	234,	234,	234,	235,	235,	235,	235,	236,	236,	236,	236,	237,	237,	237,	237,	238,	238,	238,	238,	239,	239,	239,	239,	240,	240,	240,	240,	241,	241,	241,	241,	242,	242,	242,	242,	243,	243,	243,	243,	244,	244,	244,	244,	245,	245,	245,	245,	246,	246,	246,	246,	247,	247,	247,	247,	248,	248,	248,	248,	249,	249,	249,	249,	250,	250,	250,	250,	251,	251,	251,	251,	252,	252,	252,	252,	253,	253,	253,	253,	254,	254,	254,	254,	255,	255 },
  { 255,	255,	255,	254,	254,	254,	254,	253,	253,	253,	253,	252,	252,	252,	252,	251,	251,	251,	251,	250,	250,	250,	250,	249,	249,	249,	249,	248,	248,	248,	248,	247,	247,	247,	247,	246,	246,	246,	246,	245,	245,	245,	245,	244,	244,	244,	244,	243,	243,	243,	243,	242,	242,	242,	242,	241,	241,	241,	241,	240,	240,	240,	240,	239,	239,	239,	239,	238,	238,	238,	238,	237,	237,	237,	237,	236,	236,	236,	236,	235,	235,	235,	235,	234,	234,	234,	234,	233,	233,	233,	233,	232,	232,	232,	232,	231,	231,	231,	231,	230,	230,	230,	230,	229,	229,	229,	229,	228,	228,	228,	228,	227,	227,	227,	227,	226,	226,	226,	226,	225,	225,	225,	225,	224,	224,	224,	224,	223,	223,	223,	223,	222,	222,	222,	222,	221,	221,	221,	221,	220,	220,	220,	220,	219,	219,	219,	219,	218,	218,	218,	218,	217,	217,	217,	217,	216,	216,	216,	216,	215,	215,	215,	215,	214,	214,	214,	214,	213,	213,	213,	213,	212,	212,	212,	212,	211,	211,	211,	211,	210,	210,	210,	210,	209,	209,	209,	209,	208,	208,	208,	208,	207,	207,	207,	207,	206,	206,	206,	206,	205,	205,	205,	205,	204,	204,	204,	204,	203,	203,	203,	203,	202,	202,	202,	202,	201,	201,	201,	201,	200,	200,	200,	200,	199,	199,	199,	199,	198,	198,	198,	198,	197,	197,	197,	197,	196,	196,	196,	196,	195,	195,	195,	195,	194,	194,	194,	194,	193,	193,	193,	193,	192,	192,	192,	192,	191,	191,	191,	191,	191,	190,	190,	190,	190,	189,	189,	189,	189,	188,	188,	188,	188,	187,	187,	187,	187,	186,	186,	186,	186,	185,	185,	185,	185,	184,	184,	184,	184,	183,	183,	183,	183,	182,	182,	182,	182,	181,	181,	181,	181,	180,	180,	180,	180,	179,	179,	179,	179,	178,	178,	178,	178,	177,	177,	177,	177,	176,	176,	176,	176,	175,	175,	175,	175,	174,	174,	174,	174,	173,	173,	173,	173,	172,	172,	172,	172,	171,	171,	171,	171,	170,	170,	170,	170,	169,	169,	169,	169,	168,	168,	168,	168,	167,	167,	167,	167,	166,	166,	166,	166,	165,	165,	165,	165,	164,	164,	164,	164,	163,	163,	163,	163,	162,	162,	162,	162,	161,	161,	161,	161,	160,	160,	160,	160,	159,	159,	159,	159,	158,	158,	158,	158,	157,	157,	157,	157,	156,	156,	156,	156,	155,	155,	155,	155,	154,	154,	154,	154,	153,	153,	153,	153,	152,	152,	152,	152,	151,	151,	151,	151,	150,	150,	150,	150,	149,	149,	149,	149,	148,	148,	148,	148,	147,	147,	147,	147,	146,	146,	146,	146,	145,	145,	145,	145,	144,	144,	144,	144,	143,	143,	143,	143,	142,	142,	142,	142,	141,	141,	141,	141,	140,	140,	140,	140,	139,	139,	139,	139,	138,	138,	138,	138,	137,	137,	137,	137,	136,	136,	136,	136,	135,	135,	135,	135,	134,	134,	134,	134,	133,	133,	133,	133,	132,	132,	132,	132,	131,	131,	131,	131,	130,	130,	130,	130,	129,	129,	129,	129,	128,	128,	128,	128,	127,	127,	127,	127,	127,	126,	126,	126,	126,	125,	125,	125,	125,	124,	124,	124,	124,	123,	123,	123,	123,	122,	122,	122,	122,	121,	121,	121,	121,	120,	120,	120,	120,	119,	119,	119,	119,	118,	118,	118,	118,	117,	117,	117,	117,	116,	116,	116,	116,	115,	115,	115,	115,	114,	114,	114,	114,	113,	113,	113,	113,	112,	112,	112,	112,	111,	111,	111,	111,	110,	110,	110,	110,	109,	109,	109,	109,	108,	108,	108,	108,	107,	107,	107,	107,	106,	106,	106,	106,	105,	105,	105,	105,	104,	104,	104,	104,	103,	103,	103,	103,	102,	102,	102,	102,	101,	101,	101,	101,	100,	100,	100,	100,	99,	99,	99,	99,	98,	98,	98,	98,	97,	97,	97,	97,	96,	96,	96,	96,	95,	95,	95,	95,	94,	94,	94,	94,	93,	93,	93,	93,	92,	92,	92,	92,	91,	91,	91,	91,	90,	90,	90,	90,	89,	89,	89,	89,	88,	88,	88,	88,	87,	87,	87,	87,	86,	86,	86,	86,	85,	85,	85,	85,	84,	84,	84,	84,	83,	83,	83,	83,	82,	82,	82,	82,	81,	81,	81,	81,	80,	80,	80,	80,	79,	79,	79,	79,	78,	78,	78,	78,	77,	77,	77,	77,	76,	76,	76,	76,	75,	75,	75,	75,	74,	74,	74,	74,	73,	73,	73,	73,	72,	72,	72,	72,	71,	71,	71,	71,	70,	70,	70,	70,	69,	69,	69,	69,	68,	68,	68,	68,	67,	67,	67,	67,	66,	66,	66,	66,	65,	65,	65,	65,	64,	64,	64,	64,	64,	63,	63,	63,	63,	62,	62,	62,	62,	61,	61,	61,	61,	60,	60,	60,	60,	59,	59,	59,	59,	58,	58,	58,	58,	57,	57,	57,	57,	56,	56,	56,	56,	55,	55,	55,	55,	54,	54,	54,	54,	53,	53,	53,	53,	52,	52,	52,	52,	51,	51,	51,	51,	50,	50,	50,	50,	49,	49,	49,	49,	48,	48,	48,	48,	47,	47,	47,	47,	46,	46,	46,	46,	45,	45,	45,	45,	44,	44,	44,	44,	43,	43,	43,	43,	42,	42,	42,	42,	41,	41,	41,	41,	40,	40,	40,	40,	39,	39,	39,	39,	38,	38,	38,	38,	37,	37,	37,	37,	36,	36,	36,	36,	35,	35,	35,	35,	34,	34,	34,	34,	33,	33,	33,	33,	32,	32,	32,	32,	31,	31,	31,	31,	30,	30,	30,	30,	29,	29,	29,	29,	28,	28,	28,	28,	27,	27,	27,	27,	26,	26,	26,	26,	25,	25,	25,	25,	24,	24,	24,	24,	23,	23,	23,	23,	22,	22,	22,	22,	21,	21,	21,	21,	20,	20,	20,	20,	19,	19,	19,	19,	18,	18,	18,	18,	17,	17,	17,	17,	16,	16,	16,	16,	15,	15,	15,	15,	14,	14,	14,	14,	13,	13,	13,	13,	12,	12,	12,	12,	11,	11,	11,	11,	10,	10,	10,	10,	9,	9,	9,	9,	8,	8,	8,	8,	7,	7,	7,	7,	6,	6,	6,	6,	5,	5,	5,	5,	4,	4,	4,	4,	3,	3,	3,	3,	2,	2,	2,	2,	1,	1,	1,	1,	0,	0 },
  { 0,	0,	1,	1,	2,	2,	3,	3,	4,	4,	5,	5,	6,	6,	7,	7,	8,	8,	9,	9,	10,	10,	11,	11,	12,	12,	13,	13,	14,	14,	15,	15,	16,	16,	17,	17,	18,	18,	19,	19,	20,	20,	21,	21,	22,	22,	23,	23,	24,	24,	25,	25,	26,	26,	27,	27,	28,	28,	29,	29,	30,	30,	31,	31,	32,	32,	33,	33,	34,	34,	35,	35,	36,	36,	37,	37,	38,	38,	39,	39,	40,	40,	41,	41,	42,	42,	43,	43,	44,	44,	45,	45,	46,	46,	47,	47,	48,	48,	49,	49,	50,	50,	51,	51,	52,	52,	53,	53,	54,	54,	55,	55,	56,	56,	57,	57,	58,	58,	59,	59,	60,	60,	61,	61,	62,	62,	63,	63,	64,	64,	65,	65,	66,	66,	67,	67,	68,	68,	69,	69,	70,	70,	71,	71,	72,	72,	73,	73,	74,	74,	75,	75,	76,	76,	77,	77,	78,	78,	79,	79,	80,	80,	81,	81,	82,	82,	83,	83,	84,	84,	85,	85,	86,	86,	87,	87,	88,	88,	89,	89,	90,	90,	91,	91,	92,	92,	93,	93,	94,	94,	95,	95,	96,	96,	97,	97,	98,	98,	99,	99,	100,	100,	101,	101,	102,	102,	103,	103,	104,	104,	105,	105,	106,	106,	107,	107,	108,	108,	109,	109,	110,	110,	111,	111,	112,	112,	113,	113,	114,	114,	115,	115,	116,	116,	117,	117,	118,	118,	119,	119,	120,	120,	121,	121,	122,	122,	123,	123,	124,	124,	125,	125,	126,	126,	127,	127,	128,	128,	129,	129,	130,	130,	131,	131,	132,	132,	133,	133,	134,	134,	135,	135,	136,	136,	137,	137,	138,	138,	139,	139,	140,	140,	141,	141,	142,	142,	143,	143,	144,	144,	145,	145,	146,	146,	147,	147,	148,	148,	149,	149,	150,	150,	151,	151,	152,	152,	153,	153,	154,	154,	155,	155,	156,	156,	157,	157,	158,	158,	159,	159,	160,	160,	161,	161,	162,	162,	163,	163,	164,	164,	165,	165,	166,	166,	167,	167,	168,	168,	169,	169,	170,	170,	171,	171,	172,	172,	173,	173,	174,	174,	175,	175,	176,	176,	177,	177,	178,	178,	179,	179,	180,	180,	181,	181,	182,	182,	183,	183,	184,	184,	185,	185,	186,	186,	187,	187,	188,	188,	189,	189,	190,	190,	191,	191,	192,	192,	193,	193,	194,	194,	195,	195,	196,	196,	197,	197,	198,	198,	199,	199,	200,	200,	201,	201,	202,	202,	203,	203,	204,	204,	205,	205,	206,	206,	207,	207,	208,	208,	209,	209,	210,	210,	211,	211,	212,	212,	213,	213,	214,	214,	215,	215,	216,	216,	217,	217,	218,	218,	219,	219,	220,	220,	221,	221,	222,	222,	223,	223,	224,	224,	225,	225,	226,	226,	227,	227,	228,	228,	229,	229,	230,	230,	231,	231,	232,	232,	233,	233,	234,	234,	235,	235,	236,	236,	237,	237,	238,	238,	239,	239,	240,	240,	241,	241,	242,	242,	243,	243,	244,	244,	245,	245,	246,	246,	247,	247,	248,	248,	249,	249,	250,	250,	251,	251,	252,	252,	253,	253,	254,	254,	255,	255,	255,	254,	254,	253,	253,	252,	252,	251,	251,	250,	250,	249,	249,	248,	248,	247,	247,	246,	246,	245,	245,	244,	244,	243,	243,	242,	242,	241,	241,	240,	240,	239,	239,	238,	238,	237,	237,	236,	236,	235,	235,	234,	234,	233,	233,	232,	232,	231,	231,	230,	230,	229,	229,	228,	228,	227,	227,	226,	226,	225,	225,	224,	224,	223,	223,	222,	222,	221,	221,	220,	220,	219,	219,	218,	218,	217,	217,	216,	216,	215,	215,	214,	214,	213,	213,	212,	212,	211,	211,	210,	210,	209,	209,	208,	208,	207,	207,	206,	206,	205,	205,	204,	204,	203,	203,	202,	202,	201,	201,	200,	200,	199,	199,	198,	198,	197,	197,	196,	196,	195,	195,	194,	194,	193,	193,	192,	192,	191,	191,	190,	190,	189,	189,	188,	188,	187,	187,	186,	186,	185,	185,	184,	184,	183,	183,	182,	182,	181,	181,	180,	180,	179,	179,	178,	178,	177,	177,	176,	176,	175,	175,	174,	174,	173,	173,	172,	172,	171,	171,	170,	170,	169,	169,	168,	168,	167,	167,	166,	166,	165,	165,	164,	164,	163,	163,	162,	162,	161,	161,	160,	160,	159,	159,	158,	158,	157,	157,	156,	156,	155,	155,	154,	154,	153,	153,	152,	152,	151,	151,	150,	150,	149,	149,	148,	148,	147,	147,	146,	146,	145,	145,	144,	144,	143,	143,	142,	142,	141,	141,	140,	140,	139,	139,	138,	138,	137,	137,	136,	136,	135,	135,	134,	134,	133,	133,	132,	132,	131,	131,	130,	130,	129,	129,	128,	128,	127,	127,	126,	126,	125,	125,	124,	124,	123,	123,	122,	122,	121,	121,	120,	120,	119,	119,	118,	118,	117,	117,	116,	116,	115,	115,	114,	114,	113,	113,	112,	112,	111,	111,	110,	110,	109,	109,	108,	108,	107,	107,	106,	106,	105,	105,	104,	104,	103,	103,	102,	102,	101,	101,	100,	100,	99,	99,	98,	98,	97,	97,	96,	96,	95,	95,	94,	94,	93,	93,	92,	92,	91,	91,	90,	90,	89,	89,	88,	88,	87,	87,	86,	86,	85,	85,	84,	84,	83,	83,	82,	82,	81,	81,	80,	80,	79,	79,	78,	78,	77,	77,	76,	76,	75,	75,	74,	74,	73,	73,	72,	72,	71,	71,	70,	70,	69,	69,	68,	68,	67,	67,	66,	66,	65,	65,	64,	64,	63,	63,	62,	62,	61,	61,	60,	60,	59,	59,	58,	58,	57,	57,	56,	56,	55,	55,	54,	54,	53,	53,	52,	52,	51,	51,	50,	50,	49,	49,	48,	48,	47,	47,	46,	46,	45,	45,	44,	44,	43,	43,	42,	42,	41,	41,	40,	40,	39,	39,	38,	38,	37,	37,	36,	36,	35,	35,	34,	34,	33,	33,	32,	32,	31,	31,	30,	30,	29,	29,	28,	28,	27,	27,	26,	26,	25,	25,	24,	24,	23,	23,	22,	22,	21,	21,	20,	20,	19,	19,	18,	18,	17,	17,	16,	16,	15,	15,	14,	14,	13,	13,	12,	12,	11,	11,	10,	10,	9,	9,	8,	8,	7,	7,	6,	6,	5,	5,	4,	4,	3,	3,	2,	2,	1,	1,	0,	0,	0 },
  { 0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	2,	2,	2,	2,	2,	2,	2,	3,	3,	3,	3,	3,	3,	4,	4,	4,	4,	4,	5,	5,	5,	5,	5,	6,	6,	6,	6,	7,	7,	7,	7,	8,	8,	8,	9,	9,	9,	9,	10,	10,	10,	11,	11,	11,	12,	12,	12,	13,	13,	13,	14,	14,	14,	15,	15,	15,	16,	16,	16,	17,	17,	18,	18,	18,	19,	19,	20,	20,	21,	21,	21,	22,	22,	23,	23,	24,	24,	25,	25,	25,	26,	26,	27,	27,	28,	28,	29,	29,	30,	30,	31,	31,	32,	32,	33,	33,	34,	34,	35,	36,	36,	37,	37,	38,	38,	39,	39,	40,	41,	41,	42,	42,	43,	43,	44,	45,	45,	46,	46,	47,	48,	48,	49,	49,	50,	51,	51,	52,	53,	53,	54,	55,	55,	56,	56,	57,	58,	58,	59,	60,	60,	61,	62,	62,	63,	64,	64,	65,	66,	66,	67,	68,	69,	69,	70,	71,	71,	72,	73,	73,	74,	75,	76,	76,	77,	78,	78,	79,	80,	81,	81,	82,	83,	83,	84,	85,	86,	86,	87,	88,	89,	89,	90,	91,	92,	92,	93,	94,	95,	95,	96,	97,	98,	98,	99,	100,	101,	101,	102,	103,	104,	105,	105,	106,	107,	108,	108,	109,	110,	111,	111,	112,	113,	114,	115,	115,	116,	117,	118,	118,	119,	120,	121,	122,	122,	123,	124,	125,	125,	126,	127,	128,	129,	129,	130,	131,	132,	132,	133,	134,	135,	136,	136,	137,	138,	139,	139,	140,	141,	142,	143,	143,	144,	145,	146,	146,	147,	148,	149,	149,	150,	151,	152,	153,	153,	154,	155,	156,	156,	157,	158,	159,	159,	160,	161,	162,	162,	163,	164,	165,	165,	166,	167,	168,	168,	169,	170,	171,	171,	172,	173,	173,	174,	175,	176,	176,	177,	178,	178,	179,	180,	181,	181,	182,	183,	183,	184,	185,	185,	186,	187,	188,	188,	189,	190,	190,	191,	192,	192,	193,	194,	194,	195,	196,	196,	197,	198,	198,	199,	199,	200,	201,	201,	202,	203,	203,	204,	205,	205,	206,	206,	207,	208,	208,	209,	209,	210,	211,	211,	212,	212,	213,	213,	214,	215,	215,	216,	216,	217,	217,	218,	218,	219,	220,	220,	221,	221,	222,	222,	223,	223,	224,	224,	225,	225,	226,	226,	227,	227,	228,	228,	229,	229,	229,	230,	230,	231,	231,	232,	232,	233,	233,	233,	234,	234,	235,	235,	236,	236,	236,	237,	237,	238,	238,	238,	239,	239,	239,	240,	240,	240,	241,	241,	241,	242,	242,	242,	243,	243,	243,	244,	244,	244,	245,	245,	245,	245,	246,	246,	246,	247,	247,	247,	247,	248,	248,	248,	248,	249,	249,	249,	249,	249,	250,	250,	250,	250,	250,	251,	251,	251,	251,	251,	251,	252,	252,	252,	252,	252,	252,	252,	253,	253,	253,	253,	253,	253,	253,	253,	253,	253,	253,	254,	254,	254,	254,	254,	254,	254,	254,	254,	254,	254,	254,	254,	254,	254,	254,	254,	254,	254,	254,	254,	254,	254,	254,	254,	254,	254,	254,	254,	253,	253,	253,	253,	253,	253,	253,	253,	253,	253,	253,	252,	252,	252,	252,	252,	252,	252,	251,	251,	251,	251,	251,	251,	250,	250,	250,	250,	250,	249,	249,	249,	249,	249,	248,	248,	248,	248,	247,	247,	247,	247,	246,	246,	246,	245,	245,	245,	245,	244,	244,	244,	243,	243,	243,	242,	242,	242,	241,	241,	241,	240,	240,	240,	239,	239,	239,	238,	238,	238,	237,	237,	236,	236,	236,	235,	235,	234,	234,	233,	233,	233,	232,	232,	231,	231,	230,	230,	229,	229,	229,	228,	228,	227,	227,	226,	226,	225,	225,	224,	224,	223,	223,	222,	222,	221,	221,	220,	220,	219,	218,	218,	217,	217,	216,	216,	215,	215,	214,	213,	213,	212,	212,	211,	211,	210,	209,	209,	208,	208,	207,	206,	206,	205,	205,	204,	203,	203,	202,	201,	201,	200,	199,	199,	198,	198,	197,	196,	196,	195,	194,	194,	193,	192,	192,	191,	190,	190,	189,	188,	188,	187,	186,	185,	185,	184,	183,	183,	182,	181,	181,	180,	179,	178,	178,	177,	176,	176,	175,	174,	173,	173,	172,	171,	171,	170,	169,	168,	168,	167,	166,	165,	165,	164,	163,	162,	162,	161,	160,	159,	159,	158,	157,	156,	156,	155,	154,	153,	153,	152,	151,	150,	149,	149,	148,	147,	146,	146,	145,	144,	143,	143,	142,	141,	140,	139,	139,	138,	137,	136,	136,	135,	134,	133,	132,	132,	131,	130,	129,	129,	128,	127,	126,	125,	125,	124,	123,	122,	122,	121,	120,	119,	118,	118,	117,	116,	115,	115,	114,	113,	112,	111,	111,	110,	109,	108,	108,	107,	106,	105,	105,	104,	103,	102,	101,	101,	100,	99,	98,	98,	97,	96,	95,	95,	94,	93,	92,	92,	91,	90,	89,	89,	88,	87,	86,	86,	85,	84,	83,	83,	82,	81,	81,	80,	79,	78,	78,	77,	76,	76,	75,	74,	73,	73,	72,	71,	71,	70,	69,	69,	68,	67,	66,	66,	65,	64,	64,	63,	62,	62,	61,	60,	60,	59,	58,	58,	57,	56,	56,	55,	55,	54,	53,	53,	52,	51,	51,	50,	49,	49,	48,	48,	47,	46,	46,	45,	45,	44,	43,	43,	42,	42,	41,	41,	40,	39,	39,	38,	38,	37,	37,	36,	36,	35,	34,	34,	33,	33,	32,	32,	31,	31,	30,	30,	29,	29,	28,	28,	27,	27,	26,	26,	25,	25,	25,	24,	24,	23,	23,	22,	22,	21,	21,	21,	20,	20,	19,	19,	18,	18,	18,	17,	17,	16,	16,	16,	15,	15,	15,	14,	14,	14,	13,	13,	13,	12,	12,	12,	11,	11,	11,	10,	10,	10,	9,	9,	9,	9,	8,	8,	8,	7,	7,	7,	7,	6,	6,	6,	6,	5,	5,	5,	5,	5,	4,	4,	4,	4,	4,	3,	3,	3,	3,	3,	3,	2,	2,	2,	2,	2,	2,	2,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0 },
  { 255,	253,	252,	250,	249,	247,	246,	245,	243,	242,	240,	239,	237,	236,	234,	233,	232,	230,	229,	228,	226,	225,	223,	222,	221,	219,	218,	217,	216,	214,	213,	212,	210,	209,	208,	207,	205,	204,	203,	202,	201,	199,	198,	197,	196,	195,	193,	192,	191,	190,	189,	188,	187,	186,	184,	183,	182,	181,	180,	179,	178,	177,	176,	175,	174,	173,	172,	171,	170,	169,	168,	167,	166,	165,	164,	163,	162,	161,	160,	159,	158,	157,	156,	155,	154,	153,	152,	151,	150,	149,	149,	148,	147,	146,	145,	144,	143,	142,	142,	141,	140,	139,	138,	137,	137,	136,	135,	134,	133,	133,	132,	131,	130,	129,	129,	128,	127,	126,	126,	125,	124,	123,	123,	122,	121,	120,	120,	119,	118,	118,	117,	116,	115,	115,	114,	113,	113,	112,	111,	111,	110,	109,	109,	108,	107,	107,	106,	106,	105,	104,	104,	103,	102,	102,	101,	101,	100,	99,	99,	98,	98,	97,	96,	96,	95,	95,	94,	94,	93,	93,	92,	91,	91,	90,	90,	89,	89,	88,	88,	87,	87,	86,	86,	85,	85,	84,	84,	83,	83,	82,	82,	81,	81,	80,	80,	79,	79,	78,	78,	77,	77,	76,	76,	75,	75,	75,	74,	74,	73,	73,	72,	72,	71,	71,	71,	70,	70,	69,	69,	69,	68,	68,	67,	67,	67,	66,	66,	65,	65,	65,	64,	64,	63,	63,	63,	62,	62,	62,	61,	61,	60,	60,	60,	59,	59,	59,	58,	58,	58,	57,	57,	57,	56,	56,	56,	55,	55,	55,	54,	54,	54,	53,	53,	53,	52,	52,	52,	51,	51,	51,	50,	50,	50,	50,	49,	49,	49,	48,	48,	48,	48,	47,	47,	47,	46,	46,	46,	46,	45,	45,	45,	44,	44,	44,	44,	43,	43,	43,	43,	42,	42,	42,	42,	41,	41,	41,	41,	40,	40,	40,	40,	39,	39,	39,	39,	39,	38,	38,	38,	38,	37,	37,	37,	37,	36,	36,	36,	36,	36,	35,	35,	35,	35,	35,	34,	34,	34,	34,	34,	33,	33,	33,	33,	33,	32,	32,	32,	32,	32,	31,	31,	31,	31,	31,	30,	30,	30,	30,	30,	30,	29,	29,	29,	29,	29,	29,	28,	28,	28,	28,	28,	28,	27,	27,	27,	27,	27,	27,	26,	26,	26,	26,	26,	26,	25,	25,	25,	25,	25,	25,	25,	24,	24,	24,	24,	24,	24,	24,	23,	23,	23,	23,	23,	23,	23,	22,	22,	22,	22,	22,	22,	22,	22,	21,	21,	21,	21,	21,	21,	21,	21,	20,	20,	20,	20,	20,	20,	20,	20,	19,	19,	19,	19,	19,	19,	19,	19,	19,	18,	18,	18,	18,	18,	18,	18,	18,	18,	17,	17,	17,	17,	17,	17,	17,	17,	17,	17,	16,	16,	16,	16,	16,	16,	16,	16,	16,	16,	15,	15,	15,	15,	15,	15,	15,	15,	15,	15,	15,	14,	14,	14,	14,	14,	14,	14,	14,	14,	14,	14,	14,	13,	13,	13,	13,	13,	13,	13,	13,	13,	13,	13,	13,	13,	12,	12,	12,	12,	12,	12,	12,	12,	12,	12,	12,	12,	12,	12,	11,	11,	11,	11,	11,	11,	11,	11,	11,	11,	11,	11,	11,	11,	11,	10,	10,	10,	10,	10,	10,	10,	10,	10,	10,	10,	10,	10,	10,	10,	10,	10,	9,	9,	9,	9,	9,	9,	9,	9,	9,	9,	9,	9,	9,	9,	9,	9,	9,	9,	8,	8,	8,	8,	8,	8,	8,	8,	8,	8,	8,	8,	8,	8,	8,	8,	8,	8,	8,	8,	8,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	5,	5,	5,	5,	5,	5,	5,	5,	5,	5,	5,	5,	5,	5,	5,	5,	5,	5,	5,	5,	5,	5,	5,	5,	5,	5,	5,	5,	5,	5,	5,	5,	5,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	0 }
};

byte randomWave[1024];

void setup() {

  for(int i = 0; i < 7; i++){
    pinMode(analoguePins[i], INPUT);
  }
  analogReadResolution(12);

  pinMode(trigPin, INPUT);

  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(switchPins[0], INPUT_PULLUP);
  pinMode(switchPins[1], INPUT_PULLUP);

  pinMode(ledPin, OUTPUT);

  analogReference(DEFAULT);
  pinMode(DAC0, ANALOG);

  regenerateRandomWave();

  switchChanged();

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

}

ISR(TIMER1_COMPA_vect){

  bool trigState = fastioRead(15);
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
  }
  prevTrigState = trigState;
  buttonTriggered = false;
  currentCycleCounter++;

  if(!oneShot || oneShotTriggered){
    waveSteps += waveStepsPerCycle;
  }

  if(freeRunning && waveSteps > 1023.0f){
    waveSteps = 0;
    oneShotTriggered = false;
  }

  int waveStep = round(waveSteps) % 1023;

  waveStep += phase;

  if(waveStep > 1023){
    waveStep -= 1023;
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

  if(outputValue > 255){
    outputValue = 255;
  }else if(outputValue < 0){
    outputValue = 0;
  }

  outputValue = ((outputValue * amplitude) & 65535) >> 8;

  analogWrite(DAC0, outputValue);
  analogWrite(ledPin, outputValue);
}

void calculateClockSyncWave(){
  waveStepsPerCycle = (1024.0f / ((double)cyclesPerClock * 64)) * (wholeNumbers ? round(divider) : divider);

  risesPerReset = oneShot ? 0 : round(64.0f / (float)divider);
}

void loop() {

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

  // The analogue pins are very noisy when on USB power.
  // The grounding and noise issues are much better when
  // connected to the synth power cable.
  int analogueReading = analogRead(analoguePins[readAnaloguePin]);
  if(abs(analogueReading - analogueReadings[readAnaloguePin]) > jitterThresholds[readAnaloguePin]){
    analogueReadings[readAnaloguePin] = analogueReading;
    analogueReadingsByte[readAnaloguePin] = analogueReading >> 4;

    if(readAnaloguePin == 0 || readAnaloguePin == 4){
      frequency = analogueReadingsByte[0] + analogueReadingsByte[4] + 1;
      divider = frequency;
      calculateClockSyncWave();
    }else if(readAnaloguePin == 1 || readAnaloguePin == 5){
      int newAmplitude = (analogueReadingsByte[1] + analogueReadingsByte[5]);
      amplitude = (newAmplitude > 255) ? 255 : newAmplitude;
    }else if(readAnaloguePin == 2){
      phase = analogueReadingsByte[2] * 4;
    }else if(readAnaloguePin == 3 || (switchState == 2 && readAnaloguePin == 6)){
      selectorValue = analogueReadings[3] >> 9;

      if(switchState == 2){
        selectorValue += analogueReadings[6] >> 9;
      }

      if(selectorValue > 7){
        selectorValue -= 7;
      }

      if(selectorValue == 6){
        regenerateRandomWave();
      }
    }
  }
  readAnaloguePin++;
  if(readAnaloguePin >= 7){
    readAnaloguePin = 0;
  }

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
    selectorValue = analogueReadings[3] >> 9;
  }else if(switchState == 1){
    cyclesPerClock = defaultClockCycles;
    freeRunning = true;
    oneShot = true;
    selectorValue = analogueReadings[3] >> 9;
  }else if(switchState == 2){
    cyclesPerClock = defaultClockCycles;
    freeRunning = true;
    oneShot = false;
  }

  calculateClockSyncWave();
}

void regenerateRandomWave(){

  float steps = random(4,64);
  int newValueAt = round(1024.0f / steps);

  int ticks = 0;
  byte randomValue = random(256);
  for(int i = 0; i < 1024; i++){
    randomWave[i] = randomValue;
    ticks++;
    if(ticks >= newValueAt){
      ticks = 0;
      randomValue = random(256);
    }
  }
}
