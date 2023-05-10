/*
 * Контроллер графического LCD
 * параллельный интерфейс
 * 
 *
 * ST7586S.h
 *
 * Created: 01.09.2022 14:17:00
 *  Author: Vitaliy Zinoviev
 */ 


#ifndef ST7586S_H_
#define ST7586S_H_

#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <stdlib.h>

#define ST7586S_DISPLAYSIZE_X	320
#define ST7586S_DISPLAYSIZE_Y	160

#define ST7586S_MONOCHROME_MODE 1
#define ST7586S_GRAYSCALE_MODE 0

#define ST7586S_COLOR_BLACK		3
#define ST7586S_COLOR_DARKGRAY	2
#define ST7586S_COLOR_LIGHTGRAY	1
#define ST7586S_COLOR_WHITE		0

// Font sizes (9x8, 6x8, 12x16, 15x16)
#define FONT_NORMAL 0
#define FONT_SMALL 1
#define FONT_MEDIUM 2
#define FONT_LARGE 3

// data port
#define GLCD_DATA_PORT	PORTA
#define GLCD_DATA_PIN	PINA
#define GLCD_DATA_DDR	DDRA

// control port
#define GLCD_CTRL_PORT	PORTC
#define GLCD_CTRL_PIN	PINC
#define GLCD_CTRL_DDR	DDRC

// control signals
#define GLCD_WR			PC7
#define GLCD_RD			PC6
#define GLCD_CS			PC5
#define GLCD_A0			PC4
#define GLCD_RESET		PC3

#define ST7586S_CSLOW()		cbi(GLCD_CTRL_PORT,GLCD_CS)
#define ST7586S_CSHIGH()	sbi(GLCD_CTRL_PORT,GLCD_CS)

#define ST7586S_WRLOW()		cbi(GLCD_CTRL_PORT,GLCD_WR)
#define ST7586S_WRHIGH()	sbi(GLCD_CTRL_PORT,GLCD_WR)

#define ST7586S_RDLOW()		cbi(GLCD_CTRL_PORT,GLCD_RD)
#define ST7586S_RDHIGH()	sbi(GLCD_CTRL_PORT,GLCD_RD)

#define ST7586S_CMDMODE()	cbi(GLCD_CTRL_PORT,GLCD_A0)
#define ST7586S_DATAMODE()	sbi(GLCD_CTRL_PORT,GLCD_A0)

#define ST7586S_RSTLOW()	cbi(GLCD_CTRL_PORT,GLCD_RESET)
#define ST7586S_RSTHIGH()	sbi(GLCD_CTRL_PORT,GLCD_RESET)

// All display instructions. A0 = 0 to send the instruction, 1 to send the data/arguments.
enum ST7586S_INSTRUCTION_TABLE {
	NOP=0x00,				// No Operation
	RESET=0x01,				// Software Reset
	POWERSAVE=0x10,			// Set Power Save mode (D0=0: Sleep in mode, D0=1: Sleep out mode)
	PARTIALMODE=0x12,		// Set partial mode (D0=0: Partial mode on, D0=1: Partial mode off)
	INVERSEDISPLAY=0x20,	// Set inverse display mode (D0=0: Normal display, D0=1: Inverse display)
	ALLPIXELONOFF=0x22,		// Set all pixel on mode (D0=0: All pixel off mode, D0=1: All pixel on mode)
	DISPLAYONOFF=0x28,		// Set LCD Display (D0=0: Display off, D0=1: Display on)
	SETCOLUMNADDRESS=0x2A,	// Set column address. Starting column address: 0x00=<XS<=0x7F,Ending Column address: XS=<XE<=0x7F
	SETROWADDRESS=0x2B,		// Set row address. Starting row address: 0x00=<YS=<0x9F,Ending row address: YS=<YE=<0x9F
	WRITEDISPLAYDATA=0x2C,	// Write display data to DDRAM
	READDISPLAYDATA=0x2E,	// Read display data from DDRAM (unavailable in serial interface)
	PARTIALDISPLAYAREA=0x30,// Set partial area. Partial disp. addr start: 0x00=<PTS<=0x9F, Partial disp addr end: 0x00=<PTE=<0x9F. Display Area: 64 =< Duty =< 160
	SCROLLAREA=0x33,		// Set scroll area. Top area: TA=0x00~0xA0, Scrolling Area: SA=0x00~0xA0, Bottom Area: BA=0x00~0xA0, TA+SA+BA = 160
	DISPLAYCONTROL=0x36,	// Set scan direction of COM and SEG: MY=0: COM0->COM159, MY=1: COM159->COM0, MX=0: SEG0->SEG383, MX=1: SEG383->SEG0
	STARTLINE=0x37,			// Set display start line. S=0x00~0x9F
	DISPLAYMODE=0x38,		// Set display mode. (D0=0: Gray mode, D0=1: Monochrome mode)
	ENABLEDDRAMINTERFACE=0x3A, // Enable DDRAM interface, followed by a defined argument
	ENABLEDDRAMINTERFACE_FIXARG=0x02,
	DISPLAYDUTY=0xB0,		// Set display duty DT=0x03~0x9F
	FIRSTOUTPUTCOM=0xB1,	// Set first output COM. FC=0x00~0x9F
	FOSCDIVIDER=0xB3,		// Set FOSC dividing ratio
	PARTIALDISPLAY=0xB4,	// Set partial display mode, needs a fixed argument
	PARTIALDISPLAY_FIXARG=0xA0,
	NLINEINVERSION=0xB5,	// Set N-Line inversion
	READMODIFYWRITE=0xB8,	// Read modify write control (D0=0: Enable read modify write, D0=1: Disable read modify write)
	SETVOP=0xC0,			// Set Vop
	VOPINCREASE=0xC1,		// Vop inrease one step
	VOPDECREASE=0xC2,		// Vop decrease one step
	BIASSYSTEM=0xC3,		// Set BIAS system
	BOOSTERLEVEL=0xC4,		// Set booster level
	VOPOFFSET=0xC7,			// Set Vop offset
	ANALOGCONTROL=0xD0,		// Enable analog circuit, requires fixed argument
	ANALOGCONTROL_FIXARG=0x1D,
	AUTOREADCONTROL=0xD7,	// Auto read control. On the Argument: D4=0: Enable auto read, D4=1: Disable auto read
	AUTOREADCONTROL_ARG=0x0F,
	OTPWRRDCONTROL=0xE0,	// OTP WR/RD control
	OTPCONTROLOUT=0xE1,		// OTP control out
	OTPWRITE=0xE2,			// OTP programming procedure
	OTPREAD=0xE3,			// OTP up-load procedure
	OTPSELECTIONCONTROL=0xE4,// OTP selection control. On the argument: D6=0: disable OTP, D6=1: Enable OTP
	OTPSELECTIONCONTROL_ARG=0x19,
	OPTPROGSETTING=0xE5,	// OTP programming setting, needs a fixed argument
	OPTPROGSETTING_FIXARG=0x0F,
	FRAMERATEGRAYMODE=0xF0,	// Frame rate setting in different temperature range (Gray scale mode)
	FRAMERATEMONOMODE=0xF1,	// Frame rate setting in different temperature range (Monochrome mode)
	TEMPERATURERANGE=0xF2,	// Temperature range setting
	TEMPGRADIENTCOMPENSATION=0xF4 // Set temperature gradient compensation coefficient
};

void ST7586S_Initalize(char monochrome);

void ST7586S_WriteParameter(uint8_t parameter);

void ST7586S_WriteCommand(enum ST7586S_INSTRUCTION_TABLE cmd);

void ST7586S_WriteData(uint8_t data[], uint8_t dataSize);

uint16_t ST7586S_incVOP();

uint16_t ST7586S_decVOP();

uint16_t ST7586S_setVOP(uint16_t vop);

uint16_t ST7586S_getVOP();

void ST7586S_Fill(uint8_t color);

void ST7586S_setDispPosition(uint16_t x1, uint8_t y1, uint16_t x2, uint8_t y2);

int ST7586S_WriteString(uint16_t x, uint16_t y, char *szMsg, uint8_t iSize, uint8_t iFGColor, uint8_t iBGColor);

#endif /* ST7586S_H_ */