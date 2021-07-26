/*
 * I2CDisp.c
 *
 * Created: 2021-07-21 15:13:53
 *  Author: Branden
 */ 

#include "I2CDisp.h"

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
		twiWriteSingle(port,*buffer);
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

