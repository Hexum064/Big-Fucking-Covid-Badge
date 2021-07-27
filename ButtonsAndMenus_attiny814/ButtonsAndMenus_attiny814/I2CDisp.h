/*
 * I2CDisp.h
 *
 * Created: 2021-07-21 15:14:31
 *  Author: Branden
 */ 


#ifndef I2CDISP_H_
#define I2CDISP_H_

#include <avr/io.h>

#define TWI &TWI0
#define BAUDSETTING 0x04 //<-800khz //0x12 //<-400khz //0x5E //<-100khz

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

uint8_t twiInitialize(TWI_t *port);
uint8_t errorCheck(TWI_t *port);
uint8_t twiAddress(TWI_t *port, uint8_t addr, uint8_t rw); //set rw 0 to use write address
uint8_t twiWriteSingle(TWI_t *port, uint8_t data);
uint8_t twiReadSingle(TWI_t *port, uint8_t ack);
uint8_t twiStop(TWI_t *port);
uint8_t twiReadMultiple(TWI_t *port, uint8_t *buffer, uint8_t charsToRead);
uint8_t twiWriteMultiple(TWI_t *port, uint8_t *buffer, uint8_t charsToWrite);
void send_cmd_and_stop(uint8_t cmd, uint8_t data, uint8_t address);
void init_display(uint8_t address);
void write_display(uint8_t * displayData, uint8_t address);

#endif /* I2CDISP_H_ */