
//LED PINS
const uint8_t LED_UP = 3;
const uint8_t LED_UP_LEFT = 8;
const uint8_t LED_UP_RIGHT = 2;
const uint8_t LED_MID = 9;
const uint8_t LED_LOW_LEFT = 7;
const uint8_t LED_LOW_RIGHT = 5;
const uint8_t LED_LOW = 6;
const uint8_t LED_DOT = 4;

void setupLedPins()
{
	pinMode(LED_UP,OUTPUT);
	pinMode(LED_UP_LEFT,OUTPUT);
	pinMode(LED_UP_RIGHT,OUTPUT);
	pinMode(LED_MID,OUTPUT);
	pinMode(LED_LOW_LEFT,OUTPUT);
	pinMode(LED_LOW_RIGHT,OUTPUT);
	pinMode(LED_LOW,OUTPUT);
	pinMode(LED_DOT,OUTPUT);
}

void writeNum( byte num )
{
	switch (num)
	{
		case 0:
			digitalWrite(LED_UP,          1);
			digitalWrite(LED_UP_LEFT,     1);
			digitalWrite(LED_UP_RIGHT,    1);
			digitalWrite(LED_MID,         0);
			digitalWrite(LED_LOW_LEFT,    1);
			digitalWrite(LED_LOW_RIGHT,   1);
			digitalWrite(LED_LOW,         1);
			break;
		case 1:
			digitalWrite(LED_UP,          0);
			digitalWrite(LED_UP_LEFT,     0);
			digitalWrite(LED_UP_RIGHT,    1);
			digitalWrite(LED_MID,         0);
			digitalWrite(LED_LOW_LEFT,    0);
			digitalWrite(LED_LOW_RIGHT,   1);
			digitalWrite(LED_LOW,         0);
			break;
		case 2:
			digitalWrite(LED_UP,          1);
			digitalWrite(LED_UP_LEFT,     0);
			digitalWrite(LED_UP_RIGHT,    1);
			digitalWrite(LED_MID,         1);
			digitalWrite(LED_LOW_LEFT,    1);
			digitalWrite(LED_LOW_RIGHT,   0);
			digitalWrite(LED_LOW,         1);
			break;
		case 3:
			digitalWrite(LED_UP,          1);
			digitalWrite(LED_UP_LEFT,     0);
			digitalWrite(LED_UP_RIGHT,    1);
			digitalWrite(LED_MID,         1);
			digitalWrite(LED_LOW_LEFT,    0);
			digitalWrite(LED_LOW_RIGHT,   1);
			digitalWrite(LED_LOW,         1);
			break;
		case 4:
			digitalWrite(LED_UP,          0);
			digitalWrite(LED_UP_LEFT,     1);
			digitalWrite(LED_UP_RIGHT,    1);
			digitalWrite(LED_MID,         1);
			digitalWrite(LED_LOW_LEFT,    0);
			digitalWrite(LED_LOW_RIGHT,   1);
			digitalWrite(LED_LOW,         0);
			break;
		case 5:
			digitalWrite(LED_UP,          1);
			digitalWrite(LED_UP_LEFT,     1);
			digitalWrite(LED_UP_RIGHT,    0);
			digitalWrite(LED_MID,         1);
			digitalWrite(LED_LOW_LEFT,    0);
			digitalWrite(LED_LOW_RIGHT,   1);
			digitalWrite(LED_LOW,         1);
			break;
		case 6:
			digitalWrite(LED_UP,          1);
			digitalWrite(LED_UP_LEFT,     1);
			digitalWrite(LED_UP_RIGHT,    0);
			digitalWrite(LED_MID,         1);
			digitalWrite(LED_LOW_LEFT,    1);
			digitalWrite(LED_LOW_RIGHT,   1);
			digitalWrite(LED_LOW,         1);
			break;
		case 7:
			digitalWrite(LED_UP,          1);
			digitalWrite(LED_UP_LEFT,     0);
			digitalWrite(LED_UP_RIGHT,    1);
			digitalWrite(LED_MID,         0);
			digitalWrite(LED_LOW_LEFT,    0);
			digitalWrite(LED_LOW_RIGHT,   1);
			digitalWrite(LED_LOW,         0);
			break;
		case 8:
			digitalWrite(LED_UP,          1);
			digitalWrite(LED_UP_LEFT,     1);
			digitalWrite(LED_UP_RIGHT,    1);
			digitalWrite(LED_MID,         1);
			digitalWrite(LED_LOW_LEFT,    1);
			digitalWrite(LED_LOW_RIGHT,   1);
			digitalWrite(LED_LOW,         1);
			break;
		case 9:
			digitalWrite(LED_UP,          1);
			digitalWrite(LED_UP_LEFT,     1);
			digitalWrite(LED_UP_RIGHT,    1);
			digitalWrite(LED_MID,         1);
			digitalWrite(LED_LOW_LEFT,    0);
			digitalWrite(LED_LOW_RIGHT,   1);
			digitalWrite(LED_LOW,         1);
			break;
		case 10:		//"A"
			digitalWrite(LED_UP,          1);
			digitalWrite(LED_UP_LEFT,     1);
			digitalWrite(LED_UP_RIGHT,    1);
			digitalWrite(LED_MID,         1);
			digitalWrite(LED_LOW_LEFT,    1);
			digitalWrite(LED_LOW_RIGHT,   1);
			digitalWrite(LED_LOW,         0);
			break;
		case 11:		//"b"
			digitalWrite(LED_UP,          0);
			digitalWrite(LED_UP_LEFT,     1);
			digitalWrite(LED_UP_RIGHT,    0);
			digitalWrite(LED_MID,         1);
			digitalWrite(LED_LOW_LEFT,    1);
			digitalWrite(LED_LOW_RIGHT,   1);
			digitalWrite(LED_LOW,         1);
			break;
		case 12:		//"C"
			digitalWrite(LED_UP,          1);
			digitalWrite(LED_UP_LEFT,     1);
			digitalWrite(LED_UP_RIGHT,    0);
			digitalWrite(LED_MID,         0);
			digitalWrite(LED_LOW_LEFT,    1);
			digitalWrite(LED_LOW_RIGHT,   0);
			digitalWrite(LED_LOW,         1);
			break;
		case 13:		//"d"
			digitalWrite(LED_UP,          0);
			digitalWrite(LED_UP_LEFT,     0);
			digitalWrite(LED_UP_RIGHT,    1);
			digitalWrite(LED_MID,         1);
			digitalWrite(LED_LOW_LEFT,    1);
			digitalWrite(LED_LOW_RIGHT,   1);
			digitalWrite(LED_LOW,         1);
			break;
		case 14:		//"E"
			digitalWrite(LED_UP,          1);
			digitalWrite(LED_UP_LEFT,     1);
			digitalWrite(LED_UP_RIGHT,    0);
			digitalWrite(LED_MID,         1);
			digitalWrite(LED_LOW_LEFT,    1);
			digitalWrite(LED_LOW_RIGHT,   0);
			digitalWrite(LED_LOW,         1);
			break;
		case 15:		//"F"
			digitalWrite(LED_UP,          1);
			digitalWrite(LED_UP_LEFT,     1);
			digitalWrite(LED_UP_RIGHT,    0);
			digitalWrite(LED_MID,         1);
			digitalWrite(LED_LOW_LEFT,    1);
			digitalWrite(LED_LOW_RIGHT,   0);
			digitalWrite(LED_LOW,         0);
			break;
	}
}

void writeValueDisp(uint8_t in)
{
	digitalWrite(LED_MID,         1);
	digitalWrite(LED_LOW_LEFT,    in > 8?1:0);
	digitalWrite(LED_UP_LEFT,     in > 42?1:0);
	digitalWrite(LED_UP,          in > 85?1:0);
	digitalWrite(LED_UP_RIGHT,    in > 127?1:0);
	digitalWrite(LED_LOW_RIGHT,   in > 170?1:0);
	digitalWrite(LED_LOW,         in > 212?1:0);
}

void writeSide(bool side)
{
	digitalWrite(LED_MID,         1);
	digitalWrite(LED_UP,          0);
	digitalWrite(LED_LOW,         0);
	digitalWrite(LED_LOW_LEFT,    side?0:1);
	digitalWrite(LED_UP_LEFT,     side?0:1);
	digitalWrite(LED_UP_RIGHT,    side?1:0);
	digitalWrite(LED_LOW_RIGHT,   side?1:0);
}

void writeShift()
{
	digitalWrite(LED_UP,          0);
	digitalWrite(LED_UP_LEFT,     0);
	digitalWrite(LED_UP_RIGHT,    0);
	digitalWrite(LED_MID,         1);
	digitalWrite(LED_LOW_LEFT,    0);
	digitalWrite(LED_LOW_RIGHT,   0);
	digitalWrite(LED_LOW,         0);
}

