/*
 
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

#ifndef _ADNS5050
#define _ADNS5050

#include <inttypes.h>

#undef abs
#undef round

#define NUM_PIXELS 360

class ADNS5050
{
  private:
	
	int sclkPin, sdioPin, selectPin, resetPin;
	
  public:
	
	ADNS5050(int sclk, int sdio, int _select, int _reset);
	int dx();
	int dy();
	
	unsigned char surfaceQuality();
	bool motion();

	void sync();
        
	unsigned char ADNS_read(unsigned char addr);
	void ADNS_write(unsigned char addr, unsigned char data);
	
	// NB this is not a particularly fast operation
	// don't call it more than 20 times a second
	void pixelGrab(unsigned char* fill);
        unsigned char ADNS_readPix(unsigned char addr);
	//void setResolution(int res);
	
};



#endif
