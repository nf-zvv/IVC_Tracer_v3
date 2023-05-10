/*
 * ST7586S.c
 *
 * Created: 01.09.2022 14:17:00
 *  Author: Vitaliy Zinoviev
 */ 

#include <avr/cpufunc.h>
#include <string.h>
#include "ST7586S.h"
#include "ST7586S_font.h"
#include "macro.h"


uint16_t display_vop = 0x0136;

const uint8_t pixelColorToByte[3][4] PROGMEM =
{
	{0,0x40,0x08,0xE0},
	{0,0x08,0x10,0x1C},
	{0,0x01,0x02,0x03}
};

const uint8_t columnColorToByte[4] PROGMEM = {0, 0x49, 0x92, 0xff};

void ST7586S_Initalize(char monochrome)
{
	GLCD_DATA_DDR = 0xff;
	GLCD_DATA_PORT = 0x00;
	GLCD_CTRL_DDR = 0xff;
	ST7586S_WRHIGH();
	ST7586S_RDHIGH();
	ST7586S_CSHIGH();
	ST7586S_DATAMODE();
	ST7586S_RSTHIGH();

	// Hardware Reset
	_delay_ms(60);
	ST7586S_RSTLOW();
	_delay_us(20);
	ST7586S_RSTHIGH();					// Power-ON -> reset display and wait ~120ms
	_delay_ms(120);
	
	/* Skip OTP Stuff from the datasheet, not sure if it's always required */
	//ST7586S_WriteCommand(RESET);
	//_delay_ms(120);

	ST7586S_WriteCommand(POWERSAVE | 1);			// Sleep out mode
	
	ST7586S_WriteCommand(DISPLAYONOFF);			// Disply OFF

	_delay_ms(50);

	ST7586S_WriteCommand(SETVOP);					// Set Vop to 0x0F0
	ST7586S_WriteParameter((uint8_t)(display_vop & 0x00FF));
	ST7586S_WriteParameter((uint8_t)(display_vop >> 8));

	ST7586S_WriteCommand(BIASSYSTEM);
	ST7586S_WriteParameter(0x03);				// BIAS System Ratio 1/14

	ST7586S_WriteCommand(BOOSTERLEVEL);			// Set Booster Level to x8
	ST7586S_WriteParameter(0x07);

	ST7586S_WriteCommand(ANALOGCONTROL);			// enable analog circuit
	ST7586S_WriteParameter(0x1d);

	ST7586S_WriteCommand(FOSCDIVIDER);			// Set FOSC Divider to "not divide"
	ST7586S_WriteParameter(0x00);

	ST7586S_WriteCommand(NLINEINVERSION);			// Set N-Line Inversion ( 8B, dunno why)
	ST7586S_WriteParameter(0x00);

	ST7586S_WriteCommand(DISPLAYMODE | monochrome);	// Set Display mode to grayscale or monochrome, when "monochrome" is 1

	ST7586S_WriteCommand(ENABLEDDRAMINTERFACE);	// Enable DDRAM interface
	ST7586S_WriteParameter(0x02);
	
	ST7586S_WriteCommand(DISPLAYCONTROL);			// Normal display scan direction
	ST7586S_WriteParameter(0x80);
	
	ST7586S_WriteCommand(FIRSTOUTPUTCOM);		// First output COM
	ST7586S_WriteParameter(0x00);

	ST7586S_WriteCommand(DISPLAYDUTY);			// Display duty. Set to 9F when duty is 160.
	ST7586S_WriteParameter(0x09f);
	
	ST7586S_WriteCommand(FRAMERATEGRAYMODE | monochrome);		// Display Frame Rate in Grayscale/Monochrome Mode (77Hz for all Temperatures)
	ST7586S_WriteParameter(0x12);
	ST7586S_WriteParameter(0x12);
	ST7586S_WriteParameter(0x12);
	ST7586S_WriteParameter(0x12);
	
	ST7586S_WriteCommand(INVERSEDISPLAY);			// No display inversion (Normal Display)

	ST7586S_WriteCommand(DISPLAYONOFF | 1);		// Turn display on
}

void ST7586S_WriteParameter(uint8_t parameter)
{
	ST7586S_DATAMODE();
	ST7586S_RDHIGH();
	ST7586S_CSLOW();
	GLCD_DATA_PORT = parameter;
	ST7586S_WRLOW();
	_NOP();
	_NOP();
	ST7586S_WRHIGH();
	_NOP();
	_NOP();
	ST7586S_CSHIGH();
}

void ST7586S_WriteCommand(enum ST7586S_INSTRUCTION_TABLE cmd)
{
	ST7586S_CMDMODE();
	ST7586S_RDHIGH();
	ST7586S_CSLOW();
	ST7586S_WRLOW();
	GLCD_DATA_PORT = cmd;
	_NOP();
	_NOP();
	ST7586S_WRHIGH();
	_NOP();
	_NOP();
	ST7586S_CSHIGH();
}

void ST7586S_WriteData(uint8_t data[], uint8_t dataSize)
{
	ST7586S_DATAMODE();
	ST7586S_RDHIGH();
	ST7586S_CSLOW();
	for (uint8_t i = 0; i < dataSize; i++)
	{
		ST7586S_WRLOW();
		_NOP();
		_NOP();
		GLCD_DATA_PORT = data[i];
		_NOP();
		_NOP();
		ST7586S_WRHIGH();
	}
	ST7586S_CSHIGH();
}

uint16_t ST7586S_incVOP()
{
	ST7586S_WriteCommand(VOPINCREASE);
	return (++display_vop);
}

uint16_t ST7586S_decVOP()
{
	ST7586S_WriteCommand(VOPDECREASE);
	return (--display_vop);
}

// usable VOP Voltage between 7.44V and 18V (0x0060 to 0x0168)
// Size: 9 bits!
uint16_t ST7586S_setVOP(uint16_t vop)
{
	display_vop = 0x1FF & vop;	// Only 9 bits are allowed!
	ST7586S_WriteCommand(SETVOP);					// Set Vop to 0x0F0
	ST7586S_WriteParameter((uint8_t)(display_vop & 0x00FF));
	ST7586S_WriteParameter((uint8_t)(display_vop >> 8));

	return (display_vop);
}

uint16_t ST7586S_getVOP()
{
	return (display_vop);
}

void ST7586S_Fill(uint8_t color)
{
	uint8_t fillArray[ST7586S_DISPLAYSIZE_X/3];
	/*
	for (uint8_t i = 0; i < (ST7586S_DISPLAYSIZE_X / 3); i++)
	{
		fillArray[i] = pgm_read_byte(&(columnColorToByte[color]));
	}
	*/
	memset(fillArray, pgm_read_byte(&(columnColorToByte[color])), ST7586S_DISPLAYSIZE_X/3);

	ST7586S_setDispPosition(0,0,ST7586S_DISPLAYSIZE_X-1,ST7586S_DISPLAYSIZE_Y-1);
	ST7586S_WriteCommand(WRITEDISPLAYDATA);
	for (uint8_t y = 0; y < ST7586S_DISPLAYSIZE_Y; y++)
	{
		ST7586S_WriteData(fillArray,ST7586S_DISPLAYSIZE_X/3);
	}
}

void ST7586S_setDispPosition(uint16_t x1, uint8_t y1, uint16_t x2, uint8_t y2)
{
	// Check if it's outside the display sizes, and if x1/y1 is smaller than x2/y2
	if ((x1 > ST7586S_DISPLAYSIZE_X-1) || (x2 > ST7586S_DISPLAYSIZE_X-1) || (y1 > ST7586S_DISPLAYSIZE_Y) || (y2 > ST7586S_DISPLAYSIZE_Y))
		return;
	if ((x1>x2) || (y1>y2))
		return;
	
	ST7586S_WriteCommand(SETCOLUMNADDRESS);	// Set Column Address
	ST7586S_WriteParameter(0);
	ST7586S_WriteParameter(x1/3);
	ST7586S_WriteParameter(0);
	ST7586S_WriteParameter(x2/3);
	ST7586S_WriteCommand(SETROWADDRESS);		// Set Row Address
	ST7586S_WriteParameter(0);
	ST7586S_WriteParameter(y1);
	ST7586S_WriteParameter(0);
	ST7586S_WriteParameter(y2);
}

int ST7586S_WriteString(uint16_t x, uint16_t y, char *szMsg, uint8_t iSize, uint8_t iFGColor, uint8_t iBGColor)
{
	int iLen;
	uint16_t i, j;
	uint8_t ucTemp[8], *s;
	uint8_t fgColor0, fgColor1, fgColor2, bgColor;

    if (iFGColor > 3) iFGColor = 3;
    if (iBGColor > 3) iBGColor = 3;
    if (iFGColor == 3)
    {
    	fgColor0 = 0xe0;
		fgColor1 = 0x1c;
		fgColor2 = 0x3;
    }
    else
    {
		fgColor0 = (uint8_t)iFGColor << 6; // first pixel 3-bit version of the color
		fgColor1 = (uint8_t)iFGColor << 3; // second pixel
		fgColor2 = (uint8_t)iFGColor;      // 3rd pixel
    }
    bgColor = columnColorToByte[iBGColor];
        
    iLen = strlen(szMsg);
    if (iSize == FONT_LARGE || iSize == FONT_MEDIUM) // draw 12x16 or 15x16 font
  {
  int iWidth, iDelta;
    iWidth = (iSize == FONT_LARGE) ? 15 : 12;
    iDelta = (iSize == FONT_LARGE) ? 5 : 4;
    if ((iWidth*iLen) + x > ST7586S_DISPLAYSIZE_X) iLen = (ST7586S_DISPLAYSIZE_X - x)/iWidth; // can't display it all
    if (iLen < 0)return -1;
    for (i=0; i<iLen; i++)
    {
      int tx, ty;
      uint8_t bTemp[84], bMask, bOut, bOut2, *d;
      if (iSize == FONT_LARGE)
      {
         s = (unsigned char *)&ucFont[((unsigned char)szMsg[i]-32) * 8];
         memcpy_P(ucTemp, s, 8); // copy from FLASH memory
      }
      else
      {
         s = (unsigned char *)&ucSmallFont[((unsigned char)szMsg[i]-32) * 6];
         memcpy_P(ucTemp, s, 6);
      }
       // convert from 1-bpp to 2/3-bpp
      d = bTemp;
      s = ucTemp;
      bMask = 1;
      for (ty=0; ty<8; ty++)
      {
        for (tx=0; tx<iWidth-6; tx+=3) // 3 sets of 3 pixels
        {
           bOut = bOut2 = bgColor;
           if (s[tx] & bMask)
           {
              bOut &= 0x3; // clear top 6 bits
              bOut |= fgColor0 | fgColor1; // first 2 pixels (6 bits)
           }
           if (s[tx+1] & bMask)
           {
              bOut &= 0xfc; // clear bottom 2 bits
              bOut2 &= 0x1f; // clear top 3 bits
              bOut |= fgColor2; // third pixel (2 bits)
              bOut2 |= fgColor0;
           }
           if (s[tx+2] & bMask)
           {
              bOut2 &= 0xe0; // clear lower 5 bits
              bOut2 |= fgColor1 | fgColor2; // 2nd & 3rd pixel2 of second byte
           }
           d[0] = d[iDelta] = bOut;
           if (tx != 6)
                d[1] = d[iDelta+1] = bOut2;
           d += 2;
        } // for tx
        d += 4; // skip extra line (add 4 since we incremented by 6 already)
        bMask <<= 1;
      } // for ty
      ST7586S_setDispPosition(x, y, iWidth, 16);
      ST7586S_WriteData(bTemp, 16*iDelta); // write character pattern
      x += iWidth;
    } // for each character
  } // large+medium
  else if (iSize == FONT_NORMAL)// draw 8x8 font
   {
    if ((9*iLen) + x > ST7586S_DISPLAYSIZE_X) iLen = (ST7586S_DISPLAYSIZE_X - x)/9; // can't display it all
    if (iLen < 0)return -1;

    for (i=0; i<iLen; i++)
    {
      int tx, ty;
      uint8_t bTemp[24], bMask, bOut, *d;
      s = (unsigned char *)&ucFont[((unsigned char)szMsg[i]-32) * 8];
      memcpy_P(ucTemp, s, 8); // copy from FLASH memory
       // convert from 1-bpp to 2/3-bpp
      d = bTemp;
      for (ty=0; ty<8; ty++)
      {
        bMask = 1<<ty;
        for (tx=0; tx<9; tx+=3) // 3 sets of 3 pixels
        {
           bOut = bgColor;
           if (ucTemp[tx] & bMask)
           {
              bOut &= 0x1f; // clear top 3 bits
              bOut |= fgColor0; // first pixel (3 bits)
           }
           if (ucTemp[tx+1] & bMask)
           {
              bOut &= 0xe3; // clear middle 3 bits
              bOut |= fgColor1; // second pixel (3 bits)
           }
           if (tx != 6 &&
               ucTemp[tx+2] & bMask)
           {
              bOut &= 0xfc; // clear lower 2 bits
              bOut |= fgColor2; // third pixel (2 bits)
           }
           *d++ = bOut;
        } // for tx
      } // for ty
      ST7586S_setDispPosition(x, y, 9, 8);
      x += 9;
      ST7586S_WriteData(bTemp, 24); // write character pattern
    } 
   } // normal
   else // 6x8
   {
    if ((6*iLen) + x > ST7586S_DISPLAYSIZE_X) iLen = (ST7586S_DISPLAYSIZE_X - x)/6; // can't display it all
    if (iLen < 0)return -1;

    for (i=0; i<iLen; i++)
    {
      int tx, ty;
      uint8_t bTemp[16], bMask, bOut, *d;
      s = (unsigned char *)&ucSmallFont[((unsigned char)szMsg[i]-32) * 6];
      memcpy_P(ucTemp, s, 6); // copy from FLASH memory
       // convert from 1-bpp to 2/3-bpp
      d = bTemp;
      for (ty=0; ty<8; ty++)
      {
        bMask = 1<<ty;
        for (tx=0; tx<6; tx+=3) // 2 sets of 3 pixels
        {
           bOut = bgColor;
           if (ucTemp[tx] & bMask)
           {
              bOut &= 0x1f; // clear top 3 bits
              bOut |= fgColor0; // first pixel (3 bits)
           }
           if (ucTemp[tx+1] & bMask)
           {
              bOut &= 0xe3; // clear middle 3 bits
              bOut |= fgColor1; // second pixel (3 bits)
           }
           if (ucTemp[tx+2] & bMask)
           {
              bOut &= 0xfc; // clear lower 2 bits
              bOut |= fgColor2; // third pixel (2 bits)
           }
           *d++ = bOut;
        } // for tx
      } // for ty
      ST7586S_setDispPosition(x, y, 6, 8);
      x += 6;
      ST7586S_WriteData(bTemp, 16); // write character pattern
    } 
   } // small
  return 0;
}