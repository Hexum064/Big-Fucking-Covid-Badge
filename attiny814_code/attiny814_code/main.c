/*
 * attiny814_code.c
 *
 * Created: 2021-06-29 19:07:06
 * Author : Branden
 */ 

#include <avr/io.h>
#define  F_CPU 20000000
#include "util/delay.h"

uint8_t twiInitialize(TWI_t *port);
//uint8_t twiAddress(TWI_t *port, uint8_t addr, uint8_t rw); //set rw high to read
//uint8_t twiWriteSingle(TWI_t *port, uint8_t data);
//uint8_t twiWriteMultiple(TWI_t *port, uint8_t *buffer, uint8_t charsToWrite);
//uint8_t twiReadSingle(TWI_t *port, uint8_t ack);
//uint8_t twiReadMultiple(TWI_t *port, uint8_t *buffer, uint8_t charsToRead);
//uint8_t twiStop(TWI_t *port);

#define TWI &TWI0
#define BAUDSETTING 0x04 //<-800khz //0x12 //<-400khz //0x5E //<-100khz
volatile uint8_t dummy;


#define BASE_ADDR 0xC8
#define ALL_ADDR 0x5C

#define CMD_RAMPAGE		0xFD
#define CMD_W_DISP		0x80
#define CMD_DISPMODE	0x30
#define CMD_CFGMODE		0x31
#define CMD_BRIGHT		0x37
#define CMD_CCRATIO		0x36
#define CMD_SYSCTRL		0x35
#define CMD_CASCADE		0x34
#define CMD_RESET		0xCC
#define CMD_R_STATUS	0x70
#define CMD_W_LEDCTRL	0x84

/*uint8_t reverse[256] = 
{
    0xFF,0xFE,0xFD,0xFC,0xFB,0xFA,0xF9,0xF8,0xF7,0xF6,0xF5,0xF4,0xF3,0xF2,0xF1,0xF0,
    0xEF,0xEE,0xED,0xEC,0xEB,0xEA,0xE9,0xE8,0xE7,0xE6,0xE5,0xE4,0xE3,0xE2,0xE1,0xE0,
    0xDF,0xDE,0xDD,0xDC,0xDB,0xDA,0xD9,0xD8,0xD7,0xD6,0xD5,0xD4,0xD3,0xD2,0xD1,0xD0,
    0xCF,0xCE,0xCD,0xCC,0xCB,0xCA,0xC9,0xC8,0xC7,0xC6,0xC5,0xC4,0xC3,0xC2,0xC1,0xC0,
    0xBF,0xBE,0xBD,0xBC,0xBB,0xBA,0xB9,0xB8,0xB7,0xB6,0xB5,0xB4,0xB3,0xB2,0xB1,0xB0,
    0xAF,0xAE,0xAD,0xAC,0xAB,0xAA,0xA9,0xA8,0xA7,0xA6,0xA5,0xA4,0xA3,0xA2,0xA1,0xA0,
    0x9F,0x9E,0x9D,0x9C,0x9B,0x9A,0x99,0x98,0x97,0x96,0x95,0x94,0x93,0x92,0x91,0x90,
    0x8F,0x8E,0x8D,0x8C,0x8B,0x8A,0x89,0x88,0x87,0x86,0x85,0x84,0x83,0x82,0x81,0x80,
    0x7F,0x7E,0x7D,0x7C,0x7B,0x7A,0x79,0x78,0x77,0x76,0x75,0x74,0x73,0x72,0x71,0x70,
    0x6F,0x6E,0x6D,0x6C,0x6B,0x6A,0x69,0x68,0x67,0x66,0x65,0x64,0x63,0x62,0x61,0x60,
    0x5F,0x5E,0x5D,0x5C,0x5B,0x5A,0x59,0x58,0x57,0x56,0x55,0x54,0x53,0x52,0x51,0x50,
    0x4F,0x4E,0x4D,0x4C,0x4B,0x4A,0x49,0x48,0x47,0x46,0x45,0x44,0x43,0x42,0x41,0x40,
    0x3F,0x3E,0x3D,0x3C,0x3B,0x3A,0x39,0x38,0x37,0x36,0x35,0x34,0x33,0x32,0x31,0x30,
    0x2F,0x2E,0x2D,0x2C,0x2B,0x2A,0x29,0x28,0x27,0x26,0x25,0x24,0x23,0x22,0x21,0x20,
    0x1F,0x1E,0x1D,0x1C,0x1B,0x1A,0x19,0x18,0x17,0x16,0x15,0x14,0x13,0x12,0x11,0x10,
    0x0F,0x0E,0x0D,0x0C,0x0B,0x0A,0x09,0x08,0x07,0x06,0x05,0x04,0x03,0x02,0x01,0x00,
};*/

static const uint8_t reverse[256] = 
{
#   define R2(n)     n,     n + 2*64,     n + 1*64,     n + 3*64
#   define R4(n) R2(n), R2(n + 2*16), R2(n + 1*16), R2(n + 3*16)
#   define R6(n) R4(n), R4(n + 2*4 ), R4(n + 1*4 ), R4(n + 3*4 )
    R6(0), R6(2), R6(1), R6(3)
};


volatile uint8_t _dispData[32] =
{
	0b10000000,
	0b01000000,
	0b00100000,
	0b00010000,
	0b00001000,
	0b00000100,
	0b00000010,
	0b00000001,
	0b10000000,
	0b01000000,
	0b00100000,
	0b00010000,
	0b00001000,
	0b00000100,
	0b00000010,
	0b00000001,
	0b10000000,
	0b01000000,
	0b00100000,
	0b00010000,
	0b00001000,
	0b00000100,
	0b00000010,
	0b00000001,
	0b10000000,
	0b01000000,
	0b00100000,
	0b00010000,
	0b00001000,
	0b00000100,
	0b00000010,
	0b00000001
};

volatile uint8_t _dispData2[32] =
{
	0b11111111,
	0b00000000,
	0b00000000,
	0b11111111,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	
	0b11111111,
	0b00000000,
	0b00000000,
	0b11111111,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	
	0b11111111,
	0b00000000,
	0b00000000,
	0b11111111,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	
	0b11111111,
	0b00000000,
	0b00000000,
	0b11111111,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000
};

volatile uint8_t _dispDataCircle[32] =
{
	0b11100000,	0b00000111,
	0b00010000,	0b00001000,
	0b00001000,	0b00010000,
	0b00001000,	0b00010000,
	
	0b00001000,	0b00010000,
	0b00001000,	0b00010000,
	0b00010000,	0b00001000,
	0b11100000,	0b00000111,
	
	0b00000000,	0b00000000,
	0b00000000,	0b00000000,
	0b00000000,	0b00000000,
	0b00000000,	0b00000000,
	
	0b00000000,	0b00000000,
	0b00000000,	0b00000000,
	0b00000000,	0b00000000,
	0b00000000,	0b00000000
};

volatile uint8_t _dispDataTriangleUp[32] =
{
	
	0b00000000,	0b00000000,
	0b00000000,	0b00000000,
	0b00000000,	0b00000000,
	0b00000000,	0b00000000,
	
	0b00000000,	0b00000000,
	0b00000000,	0b00000000,
	0b00000000,	0b00000000,
	0b00000000,	0b00000000,
	
	0b10000000,	0b00000001,
	0b01000000,	0b00000010,
	0b00100000,	0b00000100,
	0b00010000,	0b00001000,
	
	0b00001000,	0b00010000,
	0b00000100,	0b00100000,
	0b00000010,	0b01000000,
	0b11111111,	0b11111111
	
};

const uint8_t _dispDataTriangleUp_Reg[32] =
{
	
	0b00000000,	0b00000000,
	0b00000000,	0b00000000,
	0b00000000,	0b00000000,
	0b00000000,	0b00000000,
	
	0b00000000,	0b00000000,
	0b00000000,	0b00000000,
	0b00000000,	0b00000000,
	0b00000000,	0b00000000,
	
	0b00000001,	0b10000000,
	0b00000010,	0b01000000,
	0b00000100,	0b00100000,
	0b00001000,	0b00010000,
	
	0b00010000,	0b00001000,
	0b00100000,	0b00000100,
	0b01000000,	0b00000010,
	0b11111111,	0b11111111
	
};

volatile uint8_t _dispDataAll[32] =
{
	0b11111111,
	0b11111111,
	0b11111111,
	0b11111111,
	0b11111111,
	0b11111111,
	0b11111111,
	0b11111111,
	
	0b11111111,
	0b11111111,
	0b11111111,
	0b11111111,
	0b11111111,
	0b11111111,
	0b11111111,
	0b11111111,
	
	0b11111111,
	0b11111111,
	0b11111111,
	0b11111111,
	0b11111111,
	0b11111111,
	0b11111111,
	0b11111111,
	
	0b11111111,
	0b11111111,
	0b11111111,
	0b11111111,
	0b11111111,
	0b11111111,
	0b11111111,
	0b11111111
};

volatile uint8_t _temp[32];
volatile uint8_t _tempRev[32];

void init_clock()
{
	//Set clock to 20mhz
    CCP = 0xD8;
    CLKCTRL.MCLKCTRLB = 0x00;
	
}

uint8_t twiInitialize(TWI_t *port)
{


	port->MBAUD=(uint8_t)BAUDSETTING;
	//port->MASTER.BAUD=255;
	port->MCTRLB |= CLKCTRL_PDIV3_bm | CLKCTRL_PEN_bm;
	port->MCTRLA|=TWI_ENABLE_bm;
	port->MSTATUS = TWI_BUSSTATE0_bm; //Clear Bus State and make idle




	return 0;
}

uint8_t errorCheck(TWI_t *port)
{
	if(port->MSTATUS & TWI_BUSERR_bm
	|| port->MSTATUS & TWI_ARBLOST_bm)
	{
		port->MSTATUS = TWI_BUSSTATE0_bm;
		port->MCTRLB = TWI_MCMD_STOP_gc;
		return 255;
	}
	else
	{
		return 0;
	}
}

uint8_t twiAddress(TWI_t *port, uint8_t addr, uint8_t rw) //set rw 0 to use write address
{
	
	if(rw)
	{
		port->MADDR=addr|1;
	}
	else
	{
		port->MADDR=addr&(~1);
	}
	
	while(!(port->MSTATUS & (TWI_RIF_bm | TWI_WIF_bm)));

	//	errorCheck(port);
	


	if(port->MSTATUS & TWI_RXACK_bm)
	{
		port->MSTATUS |= TWI_RIF_bm;
		port->MSTATUS |= TWI_WIF_bm;
		port->MSTATUS = TWI_BUSSTATE0_bm;
		return 1;
	}
	else
	{

		return 0;
	}
}

uint8_t twiWriteSingle(TWI_t *port, uint8_t data)
{
	port->MDATA = data;
	while(!(port->MSTATUS & (TWI_RIF_bm | TWI_WIF_bm | TWI_CLKHOLD_bm | TWI_RXACK_bm | TWI_ARBLOST_bm | TWI_BUSERR_bm)));

	errorCheck(port);

	if(port->MSTATUS & TWI_RXACK_bm)
	{
		port->MSTATUS |= TWI_RIF_bm;
		port->MSTATUS |= TWI_WIF_bm;
		port->MSTATUS = TWI_BUSSTATE0_bm;
		return 1;
	}
	else
	{
		
		return 0;
	}
}

uint8_t twiReadSingle(TWI_t *port, uint8_t ack)
{
	//TWIC.MSTATUS |= TWI_MASTER_RIF_bm;

	while(!(port->MSTATUS & (TWI_RIF_bm | TWI_WIF_bm | TWI_CLKHOLD_bm | TWI_RXACK_bm | TWI_ARBLOST_bm | TWI_BUSERR_bm)));

	if (ack)
	{
		port->MCTRLB = TWI_MCMD_RECVTRANS_gc;
	}
	else
	{
		port->MCTRLB = TWI_MCMD_RECVTRANS_gc | TWI_ACKACT_bm;
	}

	errorCheck(port);

	return port->MDATA;
}

uint8_t twiStop(TWI_t *port)
{
	while(!(port->MSTATUS & (TWI_RIF_bm | TWI_WIF_bm | TWI_CLKHOLD_bm | TWI_RXACK_bm | TWI_ARBLOST_bm | TWI_BUSERR_bm)));

	errorCheck(port);

	port->MCTRLB = TWI_MCMD_STOP_gc;
	return 0;
}

uint8_t twiReadMultiple(TWI_t *port, uint8_t *buffer, uint8_t charsToRead)
{
	uint8_t i=0;
	for(i=0; i<(charsToRead-1); i++)
	{
		*buffer=twiReadSingle(port,1);
		buffer++;
	}
	*buffer=twiReadSingle(port,0);
	return 0;
}

uint8_t twiWriteMultiple(TWI_t *port, uint8_t *buffer, uint8_t charsToWrite)
{
	uint8_t i=0;
	for(i=0; i<charsToWrite; i++)
	{
		twiWriteSingle(port, reverse[*buffer]);
		buffer++;
	}
	return 0;
}

void send_cmd_and_stop(uint8_t cmd, uint8_t data, uint8_t address)
{
	//0x00 address for test setup
	twiAddress(TWI, address, 0); //start the frame with the address
	twiWriteSingle(TWI, cmd); //then the command
	twiWriteSingle(TWI, data); //then the data
	twiStop(TWI);
	
}

void init_display(uint8_t address)
{
	//set in this order
	//cfg mode = 0b10000000 (binary mode) | 0b00000011 (16x16)
	//dsp mode = 0: picture
	//Const Current Ratio = 0b1010 (default 33mA)
	//Global Brightness = 0b11111111 (default 255)
	//cascade mode = 0: single chip/High impedance
	//System ctrl = 0b00000011 : osc and display on
	//Mode ctrl = 0b00000000 (default all off)
	
	send_cmd_and_stop(CMD_CFGMODE, 0b10000011, address);
	send_cmd_and_stop(CMD_DISPMODE, 0, address);
	send_cmd_and_stop(CMD_CCRATIO, 0b0001, address);
	send_cmd_and_stop(CMD_BRIGHT, 0b00100000, address);
	send_cmd_and_stop(CMD_CASCADE, 0, address);
	send_cmd_and_stop(CMD_SYSCTRL, 0b00000011, address);


	
}
//
void write_display(uint8_t * displayData, uint8_t address)
{
	//send_cmd_and_stop(CMD_RAMPAGE, 0, address);
	//write address,
	//write command,
	//write ram address (0x00),
	//write data,
	//write stop
	//0x00 address for test setup
	if (!(twiAddress(TWI, address, 0))) //start the frame with the address
	{			
		twiWriteSingle(TWI, CMD_W_DISP); //then the command
		twiWriteSingle(TWI, 0x00); //then the starting ram address (0)
		twiWriteMultiple(TWI, displayData, 32); //then the data
	}
	twiStop(TWI);
}

int main(void)
{
	uint8_t i;
	uint8_t offset = 0;
	uint8_t new_i;
	init_clock();
	twiInitialize(TWI);

	
	_delay_ms(2);
	
	init_display(ALL_ADDR);
	
	_delay_ms(2);
	

	while (1)
	{
		_delay_ms(100);
		for (i = 0; i < 32; i++)
		{

			new_i = offset + i;
			if (new_i > 31)
			{
				//wrap around here
				new_i -= 32;
			}

			

			
			_temp[i] = _dispDataTriangleUp_Reg[new_i];
			//_tempRev[i] = _temp[32 - i];
			
		}
		
		for (i = 0; i < 32; i+=2)
		{
			_tempRev[30 - i] = _temp[i];
			_tempRev[31 - i] = _temp[i + 1];
		}
		write_display(_tempRev, BASE_ADDR | 0x00);
		write_display(_temp, BASE_ADDR | 0x06);

		write_display(_dispData, BASE_ADDR | 0x02);
		write_display(_dispData2, BASE_ADDR | 0x04);
		
		offset  += 2;
		
		if (offset > 31)
		{
			offset = 0;
		}

	}
}
