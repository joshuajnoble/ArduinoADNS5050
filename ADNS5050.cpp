

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
#define reset		    0x3a
#define CPI500v		    0x00
#define CPI1000v	    0x01

#define CPI500v				0x00
#define CPI1000v			0x01

/******************************************************************************
 * Constructors
 ******************************************************************************/


ADNS5050::ADNS5050(int sclkPin, int sdioPin, int _selectPin, int _resetPin )
{
  sdio = sdioPin;
  sclk = sclkPin;
  selectPin = _selectPin;
  resetPin = _resetPin;

  pinMode(sdio, OUTPUT);
  pinMode(sclk, OUTPUT);

  pinMode(resetPin, OUTPUT);
  digitalWrite(resetPin, LOW);

  pinMode(selectPin, OUTPUT);


}

/******************************************************************************
 * User API
 ******************************************************************************/

void ADNS5050::sync() {
  pinMode(selectPin, OUTPUT);
  digitalWrite(selectPin, LOW);
  delayMicroseconds(2);
  digitalWrite(selectPin, HIGH);
}

int ADNS5050::dx() { 

  byte x = ADNS_read(DELTA_X_REG);

  int xv = (int16_t)x;
  if (x & 0x80) {					// negative value
    x = ~x + 1;
    xv = (0 - (int16_t)x);
  }
  return xv;
}

int ADNS5050::dy() { 

  byte y = ADNS_read(DELTA_Y_REG);

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

void ADNS5050::ADNS_write(unsigned char addr, unsigned char data) {
  char temp;
  int n;

  digitalWrite(selectPin, LOW);//nADNSCS = 0; // select the chip

  temp = addr | 0x80;
  digitalWrite(sclk, LOW);//SCK = 0;					// start sclk low
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
    delayMicroseconds(1);	// short sclk pulse
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
    digitalWrite(sclk, HIGH);
    delayMicroseconds(1); // short sclk pulse
  }
  delayMicroseconds(10);
  digitalWrite(selectPin, HIGH); // de-select the chip
}

byte ADNS5050::ADNS_read(unsigned char addr) {
  byte temp;
  int n;

  digitalWrite(selectPin, LOW); //nADNSCS = 0;				// select the chip
  temp = addr;
  digitalWrite(sclk, OUTPUT); //SCK = 0  start clock low
  pinMode(sdio, OUTPUT); //DATA_OUT set data line for output
  for (n=0; n<8; n++) {

    digitalWrite(sclk, LOW); //SCK = 0;
    pinMode(sdio, OUTPUT); //DATA_OUT;
    if (temp & 0x80) {
      digitalWrite(sdio, HIGH); //SDOUT = 1;
    } 
    else {
      digitalWrite(sdio, LOW); //SDOUT = 0;
    }
    temp = (temp << 1);
    digitalWrite(sclk, HIGH); //SCK = 1;
  }

  temp = 0; // This is a read, switch to input
  pinMode(sdio, INPUT); //DATA_IN;
  for (n=0; n<8; n++) { // read back the data
    digitalWrite(sclk, LOW);
    if(digitalRead(sdio)) { // got a '1'
      temp |= 0x1;
    }
    if( n != 7) temp = (temp << 1); // shift left
    digitalWrite(sclk, HIGH);
  }
  delayMicroseconds(20);
  digitalWrite(selectPin, HIGH);// de-select the chip
  return temp;
}

byte ADNS5050::ADNS_read7(unsigned char addr)
{
  byte temp;
  int n;

  digitalWrite(selectPin, LOW); //nADNSCS = 0 select the chip
  temp = addr;
  digitalWrite(sclk, HIGH); //SCK = 0 start sclk low
  pinMode(sdio, OUTPUT); //DATA_OUT set data line for output

  for (n=0; n<8; n++) {

    digitalWrite(sclk, LOW); //SCK = 0;
    if (temp & 0x80) {
      digitalWrite(sdio, HIGH); //SDOUT = 1;
    } 
    else {
      digitalWrite(sdio, LOW); //SDOUT = 0;
    }
    temp = (temp << 1);
    digitalWrite(sclk, HIGH); //SCK = 1;
    // short sclk pulse
  }

  temp = 0; // This is a read, switch to input

  pinMode(sdio, INPUT); //DATA_IN;
  for (n=0; n < 7; n++) { // read back the data
    digitalWrite(sclk, LOW);
    delayMicroseconds(10);
    if(digitalRead(sdio)) { // got a '1'
      temp |= 0x1;
    }
    digitalWrite(sclk, HIGH);
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
    fill[grabCount] = ADNS_read7(PIXEL_DATA_REG);
    grabCount++;
  }

}

