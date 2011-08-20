

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
#define RESET		    0x3a
#define CPI500v		    0x00
#define CPI1000v	    0x01

#define CPI500v				0x00
#define CPI1000v			0x01

/******************************************************************************
 * Constructors
 ******************************************************************************/


ADNS5050::ADNS5050(uint8_t sclkPin, uint8_t sdioPin, unint8_t selectPin, uint8_t resetPin )
{
	sdio = sdioPin;
	sclk = sclkPin;
	select = selectPin;
	reset = resetPin;
	
	pinMode(sdio, OUTPUT);
	pinMode(sclk, OUTPUT);
	
	pinMode(reset, OUTPUT);
	digitalWrite(RESET, LOW);
	
	pinMode(select, OUTPUT);
	
	
}

/******************************************************************************
 * User API
 ******************************************************************************/

void sync() {
	pinMode(select, OUTPUT);
	digitalWrite(select, LOW);
	delayMicroseconds(2);
	digitalWrite(select, HIGH);
}

byte ADNS5050::dx() { 
	return ADNS_read(DELTA_X_REG);
}

byte ADNS5050::dy() { 
	return ADNS_read(DELTA_Y_REG);
}

byte ADNS5050::surfaceQuality() {
	return ADNS_read(SQUAL_REG);

void ADNS5050::ADNS_write(unsigned char addr, unsigned char data) {
	char temp;
	int n;
	
	digitalWrite(select, LOW);//nADNSCS = 0; // select the chip
	
	temp = addr;
	digitalWrite(sclk, LOW);//SCK = 0;					// start clock low
	pinMode(sdio, OUTPUT);//DATA_OUT; // set data line for output
	for (n=0; n<8; n++) {
		digitalWrite(sclk, LOW);//SCK = 0;
		pinMode(sdio, OUTPUT);
		delayMicroseconds(1);
		if (temp & 0x80)
			digitalWrite(sdio, HIGH);//SDOUT = 1;
		else
			digitalWrite(sdio, LOW);//SDOUT = 0;
		temp = (temp << 1);
		digitalWrite(sclk, HIGH);//SCK = 1;
		delayMicroseconds(1);//delayMicroseconds(1);			// short clock pulse
	}
	temp = data;
	for (n=0; n<8; n++) {
		digitalWrite(sclk, LOW);//SCK = 0;
		delayMicroseconds(1);
		if (temp & 0x80)
			digitalWrite(sdio, HIGH);//SDOUT = 1;
		else
			digitalWrite(sdio, LOW);//SDOUT = 0;
		temp = (temp << 1);
		digitalWrite(sclk, HIGH);//SCK = 1;
		delayMicroseconds(1);			// short clock pulse
	}
	delayMicroseconds(20);
	digitalWrite(select, HIGH);//nADNSCS = 1; // de-select the chip
}

byte ADNS5050::ADNS_read(unsigned char addr) {
	byte temp;
	int n;
	
	digitalWrite(select, LOW);//nADNSCS = 0;				// select the chip
	temp = addr;
	digitalWrite(sclk, OUTPUT); //SCK = 0;					// start clock low
	pinMode(sdio, OUTPUT); //DATA_OUT;					// set data line for output
	for (n=0; n<8; n++) {
		
		digitalWrite(sclk, LOW);//SCK = 0;
		pinMode(sdio, OUTPUT); //DATA_OUT;
		if (temp & 0x80) {
			digitalWrite(sdio, HIGH);//SDOUT = 1;
		} 
		else {
			digitalWrite(sdio, LOW);//SDOUT = 0;
		}
		temp = (temp << 1);
		delayMicroseconds(1);
		digitalWrite(sclk, HIGH); //SCK = 1;
		delayMicroseconds(1);			// short clock pulse
	}
	
	temp = 0; // This is a read, switch to input
	pinMode(sdio, INPUT); //DATA_IN;
	for (n=0; n<8; n++) {		// read back the data
		digitalWrite(sclk, LOW);
		if(digitalRead(sdio)) {// got a '1'
			temp |= 0x1;
		}
		if( n != 7) temp = (temp << 1); // shift left
		digitalWrite(sclk, HIGH);
	}
	
	digitalWrite(select, HIGH);// de-select the chip
	return temp;
}

void ADNS5050::pixelGrab(unsigned char *fill)
{
	
	int grabCount = 0; 
	while( grabCount < NUM_PIXELS )
	{
		pix[grabCount] = ADNS_read(PIXEL_DATA_REG);
		grabCount++;
	}
	
}
