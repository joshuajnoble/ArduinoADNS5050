

#include "WConstants.h"
#include "ADNS5050.h"

/******************************************************************************
 * Definitions
 ******************************************************************************/

#define PRODUCT_ID          0x00 // should be 0x12
#define PRODUCTID2          0x3e
#define REVISION_ID         0x01
#define DELTA_Y_REG         0x03
#define DELTA_X_REG         0x04
#define SQUAL_REG           0x05
#define MAXIMUM_PIXEL_REG   0x08
#define MINIMUM_PIXEL_REG   0x0a
#define PIXEL_SUM_REG       0x09
#define PIXEL_DATA_REG      0x0b
#define SHUTTER_UPPER_REG   0x06
#define SHUTTER_LOWER_REG   0x07
#define MOUSE_CONTROL       0x0D
#define RESET				0x3a
#define CPI500v				0x00
#define CPI1000v			0x01

/******************************************************************************
 * Constructors
 ******************************************************************************/

#ifdef __AVR_AT90USB1286__ 
#define MICROSEC_DELAY      5
#else
#define MICROSEC_DELAY      2
#endif


ADNS5050::ADNS5050(int sclk, int sdio, int _select, int _reset )
{
  sdioPin = sdio;
  sclkPin = sclk;
  selectPin = _select;
  resetPin = _reset;

  pinMode(sdioPin, OUTPUT);
  pinMode(sclkPin, OUTPUT);

  pinMode(resetPin, OUTPUT);
  digitalWrite(resetPin, HIGH);


}

/******************************************************************************
 * User API
 ******************************************************************************/

void ADNS5050::sync() {
  
  pinMode(selectPin, OUTPUT);
  digitalWrite(selectPin, LOW);
  delayMicroseconds(2);
  digitalWrite(selectPin, HIGH);
  
  ADNS_write(RESET, 0x5a);
  delay(100); // From NRESET pull high to valid mo tion, assuming VDD and motion is present.
  
  ADNS_write(MOUSE_CONTROL, 0x01);
}

int ADNS5050::dx() { 

  byte x = ADNS_read(DELTA_X_REG);
  
  if( x == 0 ) return 0;

  int xv = (int16_t)x;
  if (x & 0x80) {					// negative value
    x = ~x + 1;
    xv = (0 - (int16_t)x);
  }
  return xv;
}

int ADNS5050::dy() { 

  byte y = ADNS_read(DELTA_Y_REG);
  
  if( y == 0 ) return 0;

  int yv = (int16_t)y;
  if (y & 0x80) {					// negative number
    y = ~y + 1;
    yv = (0 - (int16_t)y);
  }

  return yv;
}

byte ADNS5050::surfaceQuality() {
  return ADNS_read(SQUAL_REG);
}

void ADNS5050::ADNS_write(unsigned char addr, unsigned char data) 
{
	char temp;
	int n;
	
	digitalWrite(selectPin, LOW);//nADNSCS = 0; // select the chip
	
	temp = addr | 0x80; // MOST RECENT
	
	digitalWrite(sclkPin, LOW); //SCK = 0;					// start clock low
	pinMode(sdioPin, OUTPUT); //DATA_OUT; // set data line for output
	for (n=0; n<8; n++) {
		digitalWrite(sclkPin, LOW); //SCK = 0;
		pinMode(sdioPin, OUTPUT);
		delayMicroseconds(MICROSEC_DELAY);
		if (temp & 0x80)
			digitalWrite(sdioPin, HIGH); //SDOUT = 1;
		else
			digitalWrite(sdioPin, LOW);//SDOUT = 0;
		temp = (temp << 1);
		digitalWrite(sclkPin, HIGH);//SCK = 1;
		delayMicroseconds(MICROSEC_DELAY);//delayMicroseconds(1);			// short clock pulse
	}
	temp = data;
	for (n=0; n<8; n++) {
		delayMicroseconds(MICROSEC_DELAY);
		digitalWrite(sclkPin, LOW);//SCK = 0;
		delayMicroseconds(MICROSEC_DELAY);
		if (temp & 0x80)
			digitalWrite(sdioPin, HIGH);//SDOUT = 1;
		else
			digitalWrite(sdioPin, LOW);//SDOUT = 0;
		temp = (temp << 1);
		digitalWrite(sclkPin, HIGH);//SCK = 1;
		delayMicroseconds(1);			// short clock pulse
	}
	delayMicroseconds(20);
	digitalWrite(selectPin, HIGH); //nADNSCS = 1; // de-select the chip
}

byte ADNS5050::ADNS_read(unsigned char addr)
{
	byte temp;
	int n;
	
	digitalWrite(selectPin, LOW); //nADNSCS = 0;				// select the chip
	temp = addr;
	digitalWrite(sclkPin, OUTPUT); //SCK = 0  start clock low
	pinMode(sdioPin, OUTPUT); //DATA_OUT set data line for output
	for (n=0; n<8; n++) {
		delayMicroseconds(MICROSEC_DELAY);
		digitalWrite(sclkPin, LOW); //SCK = 0;
		delayMicroseconds(MICROSEC_DELAY);
		pinMode(sdioPin, OUTPUT); //DATA_OUT;
		if (temp & 0x80) {
			digitalWrite(sdioPin, HIGH); //SDOUT = 1;
		} 
		else {
			digitalWrite(sdioPin, LOW); //SDOUT = 0;
		}
		delayMicroseconds(MICROSEC_DELAY);
		temp = (temp << 1);
		digitalWrite(sclkPin, HIGH); //SCK = 1;
	}
	
	temp = 0; // This is a read, switch to input
	pinMode(sdioPin, INPUT); //DATA_IN;
	for (n=0; n<8; n++) { // read back the data
		delayMicroseconds(MICROSEC_DELAY);
		digitalWrite(sclkPin, LOW);
		delayMicroseconds(MICROSEC_DELAY);
		if(digitalRead(sdioPin)) { // got a '1'
			temp |= 0x1;
		}
		if( n != 7) temp = (temp << 1); // shift left
		digitalWrite(sclkPin, HIGH);
	}
	delayMicroseconds(20);
	digitalWrite(selectPin, HIGH);// de-select the chip
	return temp;
}

unsigned char ADNS5050::ADNS_readPix(unsigned char addr)
{
	byte temp;
	int n;
	
	digitalWrite(selectPin, LOW); //nADNSCS = 0 select the chip
	temp = addr;
	digitalWrite(sclkPin, HIGH); //SCK = 0 start clock low
	pinMode(sdioPin, OUTPUT); //DATA_OUT set data line for output
	
	for (n=0; n<8; n++) {
		digitalWrite(sclkPin, LOW); //SCK = 0;
		delayMicroseconds(MICROSEC_DELAY);
		if (temp & 0x80) {
			digitalWrite(sdioPin, HIGH); //SDOUT = 1;
		} 
		else {
			digitalWrite(sdioPin, LOW); //SDOUT = 0;
		}
		temp = (temp << 1);
		digitalWrite(sclkPin, HIGH); //SCK = 1;
		delayMicroseconds(MICROSEC_DELAY); // short clock pulse
	}
	
	temp = 0; // This is a read, switch to input
	
	pinMode(sdioPin, INPUT); //DATA_IN;
	for (n=0; n < 7; n++) { // read back the data
		digitalWrite(sclkPin, LOW);
		delayMicroseconds(MICROSEC_DELAY);
		if(digitalRead(sdioPin)) { // got a '1'
			temp |= 0x1;
		}
		digitalWrite(sclkPin, HIGH);
		delayMicroseconds(MICROSEC_DELAY);
		if( n != 6 ) temp = (temp << 1); // shift left
	}
	
	digitalWrite(selectPin, HIGH);// de-select the chip
	return temp;
}

void ADNS5050::pixelGrab(unsigned char *fill)
{

  int grabCount = 0; 
  while( grabCount < NUM_PIXELS )
  {
    fill[grabCount] = ADNS_readPix(PIXEL_DATA_REG);
    grabCount++;
  }

}

