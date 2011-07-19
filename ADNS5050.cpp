/*
 Based on sketches by Benoît Rousseau.
 
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

/******************************************************************************
 * Includes
 ******************************************************************************/

#include "WConstants.h"
#include "OptiMouse.h"
#include "ADNS5050.h"

/******************************************************************************
 * Definitions
 ******************************************************************************/

#define MOTION				0x02
#define Delta_Y				0x03
#define Delta_X				0x04
#define SQUAL				0x05
#define PIXEL_GRAB			0x0b
#define RESOLUTION			0x0c

#define CPI500v				0x00
#define CPI1000v			0x01

/******************************************************************************
 * Constructors
 ******************************************************************************/


ADNS5050::ADNS5050(uint8_t sclkPin, uint8_t sdioPin) : OptiMouse::OptiMouse(sclkPin, sdioPin)
{

}

/******************************************************************************
 * User API
 ******************************************************************************/

signed char ADNS5050::dx(void)
{
	return (signed char) readRegister(Delta_X);
}

signed char ADNS5050::dy(void)
{
	return (signed char) readRegister(Delta_Y);
}

unsigned char ADNS5050::surfaceQuality()
{
	return (unsigned char) readRegister(SQUAL);
}

bool ADNS5050::motion()
{
	return (bool) readRegister(MOTION);
}

void ADNS5050::pixelGrab(unsigned char* fill)
{
	int i = 7;
	uint8_t r = 0;
	
	// Write the address of the register we want to read:
	pinMode (_sdioPin, OUTPUT);
	for (; i>=0; i--)
	{
		digitalWrite (_sclkPin, LOW);
		digitalWrite (_sdioPin, PIXEL_GRAB & (1 << i));
		digitalWrite (_sclkPin, HIGH);
	}
	
	// Switch data line from OUTPUT to INPUT
	pinMode (_sdioPin, INPUT);
	
	// Wait a bit...
	delayMicroseconds(100);
	int j;
	for(j=0; j<360; j++)
	{	
		// Fetch the data!
		for (i=7; i>=0; i--)
		{                             
			digitalWrite (_sclkPin, LOW);
			digitalWrite (_sclkPin, HIGH);
			r |= (digitalRead (_sdioPin) << i);
		}
		delayMicroseconds(100);
		fill[j] = r;
		
	}
}

void ADNS5050::setResolution(int res)
{

	if(res == 500)
	{
		writeRegister(RESOLUTION, CPI500v);
	}
	else if(res == 1000)
	{
		writeRegister(RESOLUTION, CPI1000v);
	}

}

