/*
 * WriteToFlash_Attin814.c
 *
 * Created: 2021-07-20 14:11:06
 * Author : Branden
 */ 

//#define PAGESIZE 256
#include <avr/io.h>
#define  F_CPU 20000000
//#include "I2CDisp.h"
//#include "util/delay.h"
//#include "5x5_font.h"
//#include "images.h"

#define WIDTH 16
#define HEIGHT 16
#define FONT_W 5
#define FONT_H 5

#define FLAST_START 0x8000 //Flash mem starting address
#define SLOT_START (FLAST_START + (0x2000 - 0x440)) //Starting address for slots in APPDATA
//1C00 in hex is 7k from the start of Flash mem

volatile uint8_t *data_ptr = (uint8_t *)SLOT_START;
//volatile uint8_t data = MAPPED_PROGMEM_PAGE_SIZE ;
uint8_t board[128];
//uint8_t disp[32];

void init_clock()
{
	//Set clock to 20mhz
	CCP = CCP_IOREG_gc;
	CLKCTRL.MCLKCTRLB = 0x00;
	
}

void read_slot_8(uint8_t slot, uint8_t * destBoard)
{
	uint8_t * address = SLOT_START + (slot * 128);

	for (uint8_t i = 0; i < 64; i++)
	{
		destBoard[i] = address[i];
	}
	
}


void write_slot_8(uint8_t slot, uint8_t * sourceBoard)
{

	uint8_t temp;
	//Slots are 128 bytes long
	uint8_t *pageStart = SLOT_START + (slot * 128);
	uint8_t *slotStart = pageStart;
	uint8_t i;
	
	_PROTECTED_WRITE_SPM(NVMCTRL.CTRLA, NVMCTRL_CMD_PAGEBUFCLR_gc);
	while((NVMCTRL.STATUS & (NVMCTRL_EEBUSY_bm |  NVMCTRL_FBUSY_bm)));

	for (i = 0; i < 64; i++)
	{

		//Update the page buff with the source board data
		temp = sourceBoard[i];
		//using temp for debugging
		slotStart[i] = temp;

	}
	
	_PROTECTED_WRITE_SPM(NVMCTRL.CTRLA, NVMCTRL_CMD_PAGEERASEWRITE_gc);
	while((NVMCTRL.STATUS & (NVMCTRL_EEBUSY_bm |  NVMCTRL_FBUSY_bm)));
	
	for (i = 64; i < 128; i++)
	{

		//Update the page buff with the source board data
		temp = sourceBoard[i];
		//using temp for debugging
		slotStart[i] = temp;

	}
	
	_PROTECTED_WRITE_SPM(NVMCTRL.CTRLA, NVMCTRL_CMD_PAGEERASEWRITE_gc);
	while((NVMCTRL.STATUS & (NVMCTRL_EEBUSY_bm |  NVMCTRL_FBUSY_bm)));

}
//
//void draw_char(uint8_t x, uint8_t y, uint8_t chr, uint8_t * board)
//{
	//uint8_t index = 0; //index in board array
	//uint8_t pos = 0; //bit pos in board[index]
	//uint8_t chrByte;
	//
	////Make sure the x and y leave enough room for the character
	//if (x + FONT_W > WIDTH
		//|| y + FONT_H > HEIGHT)
	//{			
		//return;
	//}
	//
	////Then get the starting board index and bit pos
	//// we will assume the WIDTH is byte aligned.
	//index = ((y * WIDTH) + x) / 8; //overall xy pos / 8 should be the index;
	//pos = (x % 8); //  x % 8 should give bit pos. 7 - pos because we will count down from the MSB
	//
	//if (chr < 0x20 || chr > 0x7F)
	//{
		//chr = 0x20;
	//}
	//
	//chr -= 0x20;
	//
	////load the character bytes
	////set the bits in the board
	//for (uint8_t i = 0; i < FONT_W; i++)
	//{
		//chrByte = pgm_read_byte(font + (chr * FONT_W) + i);	
		//
		////Cycle through the bits of the font
		//for (uint8_t j = 0; j < FONT_H; j++)
		//{
			////fonts are rotated CC 90. 
			//
			////This for loop moves down bytes in the board
			////and up BITS in the current char byte
			//
//
			//
			//if ((chrByte & (1 << j)))
			//{
				////set byte
				//board[index + (j * (WIDTH/8))] |=  1 << pos;
			//}
			//else
			//{
				////clear byte
				//board[index + (j * (WIDTH/8))] &= ~(1 << pos);
			//}				
		//}
		//
		//pos++; //Assuming that we are always inc i by only 1.
		//
		//if (pos > 7)
		//{			
			////if we reached here, it means we looped around so we need
			////to bump up the offset for the board index
			////should only happen once per wrap around
			//pos = 0;			
			//index++;
		//}
		//
		//
	//}
	//
//
	//
//
//}
//
//void draw_string(uint8_t x, uint8_t y, uint8_t * string, uint8_t * board)
//{
	//uint8_t i = 0;
	//uint8_t cursor = x;
	//while (string[i] != '\0')
	//{
		//draw_char(cursor, y, string[i], board);
		//i++;
		//cursor += FONT_W;
	//}
//
//}


int main(void)
{
	//CLock setup	
	init_clock();
		
		
	//Display setup and test
	//for(uint8_t i = 0; i < 128; i+=2)
	//{
		////disp[i] = pgm_read_byte(nyan1 + (i * 4));
		////disp[i + 1] = pgm_read_byte(nyan1 + (i * 4) + 1);
		//
	//}
		
	//twiInitialize(TWI);
	//
	//_delay_ms(2);
		//
	//init_display(ALL_ADDR);
//
	//_delay_ms(2);

	//write_display(disp, BASE_ADDR | 0x00);
	//
	//_delay_ms(2000);
	
	
	//draw_string(1, 3, "Hi!", disp);
	//write_display(disp, BASE_ADDR | 0x00);
	//
	//_delay_ms(500);
	//draw_string(1, 9, "All!", disp);
	//write_display(disp, BASE_ADDR | 0x00);
	//
	//_delay_ms(2000);
	//
	//
	//for(uint8_t i = 0; i < 32; i++)
	//{
		//disp[i] = 0;//i;
	//}
		//
	//write_display(disp, BASE_ADDR | 0x00);
		//
	//uint8_t startChar = 65;
	
	
	//Flash read and write test
	//PORTB.DIRSET = 0x03;

	read_slot_8(0, board);
	//PORTB.OUTSET = 0x03 & board[1];

	for (uint8_t i = 0; i < 128; i++)
	{
		board[i] = i;
	}

	write_slot_8(0, board);

	
	for (uint8_t i = 0; i < 128; i++)
	{
		board[i] = 127 - i;
	}
	
	write_slot_8(1, board);
	
	read_slot_8(0, board);
	
	read_slot_8(1, board);
	data_ptr++;	
	
	//while(1)
	//{
		//if (startChar > 0x7E)
		//{
			//startChar = 0x20;
		//}
	//
		////Preprogrammed Flash read test
		////uint8_t *chr = 0;
	////
		////for (uint8_t i = 0; i < 5; i++)
		////{
		////
			////disp[i<<1] = pgm_read_byte(font + (4-i) + (startChar * 5));
		////}
//
		//
		//draw_char(0,0, startChar , disp);
		//draw_char(6,6, startChar , disp);
		//write_display(disp, BASE_ADDR | 0x00);
	//
		//startChar++;
		//
		//_delay_ms(500);
	//
	//}
	 

   while(1);
}

//data_ptr++;
	////data_ptr[0] = 0x01;
	//data = data_ptr[0];
	//PORTB.OUTSET = 0x03 & data;
	//data_ptr[0] = 0x01;
	//
	//
	//data_ptr[1] = 0x02;
//
	////if(!((uint16_t)data_ptr % MAPPED_PROGMEM_PAGE_SIZE)) {
		///* Page boundary reached, Commit page to Flash */
		//_PROTECTED_WRITE_SPM(NVMCTRL.CTRLA, NVMCTRL_CMD_PAGEERASEWRITE_gc);
		//while(NVMCTRL.STATUS & NVMCTRL_FBUSY_bm);
	////}
//
	//data = data_ptr[1];
//
	////*data_ptr = 0x01;
	////_PROTECTED_WRITE_SPM(NVMCTRL.CTRLA, NVMCTRL_CMD_PAGEERASEWRITE_gc);
	////while(NVMCTRL.STATUS & NVMCTRL_FBUSY_bm);
		//
	//data = data_ptr[0];
	//data++;
	
	
		////First read in the existing data for the slot not being updated
		//
		////If slot is even, we are updating the lower portion of the page
		////so we need to read in the upper
		//if ((slot % 2))
		//{
		////Slot is in upper half of page.
		//slotStart += 0;
		//}
		//else
		//{
		//pageStart += 0;
		//}