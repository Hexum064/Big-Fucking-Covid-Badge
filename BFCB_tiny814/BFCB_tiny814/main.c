/*
 * File:   main.c
 * Author: Branden
 *
 * Created on July 13, 2021, 3:24 PM
 */


#include <avr/io.h>
#define  F_CPU 20000000
#include "util/delay.h"
#include <avr/pgmspace.h> 
#include "images.h"
#include "I2CDisp.h"
#include "5x5_font.h"
#include "patterns.h"

#define CELL_DIE_MIN 2
#define CELL_DIE_MAX 3
#define CELL_BORN CELL_DIE_MAX

#define  MAX_SLOTS			8

#define WIDTH_BYTES			4
#define HEIGHT_BYTES		32
#define WIDTH				WIDTH_BYTES * 8
#define HEIGHT				HEIGHT_BYTES
#define FONT_W				5
#define FONT_H				5

#define EDIT_CURSOR_SPEED	30
#define BASE_COUNT			10
#define DEBOUNCE_COUNT		5
#define LONG_HOLD_COUNT		100
#define REPEAT_PAUSE_COUNT	1000
#define BUTTON_PORT			PORTA
#define DOWN_BUTTON_bm		0b00001000
#define LEFT_BUTTON_bm		0b00010000
#define RIGHT_BUTTON_bm		0b00100000
#define UP_BUTTON_bm		0b01000000
#define SELECT_BUTTON_bm	0b10000000
#define ALL_BUTTONS_bm		(DOWN_BUTTON_bm | LEFT_BUTTON_bm | RIGHT_BUTTON_bm | UP_BUTTON_bm | SELECT_BUTTON_bm)
#define DIR_BUTTONS_bm		(DOWN_BUTTON_bm | LEFT_BUTTON_bm | RIGHT_BUTTON_bm | UP_BUTTON_bm) //0b01111000

#define FLAST_START 0x8000 //Flash mem starting address
#define SETTINGS_START 0x1400//(0x9BC0) //EEPROM
#define SLOT_START (0x9C00) //FLASH



typedef enum
{
	GAME = 0x00,
	MENU = 0x01,
	EDIT = 0x02
} system_state_enum;

typedef enum
{
	NORMAL = 0x00,
	NYAN_CAT = 0x01
} game_mode_enum;

typedef enum
{
	REPEAT = 0x00,
	STOP = 0x01
} game_loop_mode_enum;

typedef enum
{
	GAME_SPEED,
	BRIGHTNESS,
	NYAN_CAT_MODE,
	LOOP_MODE,
	SLOT_SELECT,
	SLOT_EDIT,
	SLOT_SAVE,
} menu_state_enum;

typedef enum
{
	SAVE,
	CANCEL,
	BACK,
	RESET,
} slot_save_menu_options_enum;

typedef enum
{
	SHORT_SELECT = 0x80, // SELECT_BUTTON_bm,
	LONG_SELECT = 0x81,// SELECT_BUTTON_bm | 0x01,
	UP_CLICK = 0x40, //UP_BUTTON_bm,
	DOWN_CLICK = 0x08, //DOWN_BUTTON_bm,
	LEFT_CLICK = 0x10,//LEFT_BUTTON_bm,
	RIGHT_CLICK = 0x20// RIGHT_BUTTON_bm
} command_enum;

typedef enum
{
	LEVEL_1,
	LEVEL_2,
	LEVEL_3,
	LEVEL_4,
	LEVEL_5,
	LEVEL_6,
	LEVEL_7,
	LEVEL_8
	
} brightness_level_enum;


typedef enum
{
	SPEED_1,
	SPEED_2,
	SPEED_3,
	SPEED_4,
	SPEED_5,
	SPEED_6,
	SPEED_7,
	SPEED_8
	
	
} game_speed_enum;

volatile int8_t board_0[128];
volatile int8_t board_1[128];

volatile game_speed_table[] = {100, 75, 50, 40, 20, 10, 5, 2};
volatile brightness_level_tabel[] = {0x08, 0x18, 0x20, 0x2F, 0x40, 0x4F, 0x80, 0xA0};

volatile uint8_t game_speed_counter = 0;
volatile uint8_t button_debounce_counter = 0;
volatile uint8_t button_long_hold_counter = 0;
volatile uint8_t repeat_pause_count = 0;
volatile uint8_t pressed_button = 0;
volatile uint8_t selected_button = 0;

volatile game_speed_enum game_speed;
volatile system_state_enum system_state;
volatile menu_state_enum menu_state;
volatile brightness_level_enum brightness_level;
volatile game_mode_enum game_mode;
volatile game_loop_mode_enum game_loop_mode;
volatile slot_save_menu_options_enum slot_save_menu_option;
volatile uint8_t game_slot;
volatile uint8_t edit_slot;
volatile uint8_t nyan_cat_index = 0;
volatile uint8_t neighbors = 0;
volatile uint8_t saved_slots = 0; //each bit indicates a saved slot
volatile uint8_t edit_cursor_x = 0;
volatile uint8_t edit_cursor_y = 0;
volatile uint8_t cursor_highlight = 0; //show positive or negative of cursor

uint8_t * sourceBoard = board_0;
uint8_t * targetBoard = board_1;

void apply_game_slot(uint8_t slot);

void init_clock()
{
	//Set clock to 20mhz
	CCP = CCP_IOREG_gc;
	CLKCTRL.MCLKCTRLB = 0x00;
	
}

void init_rtc_clock()
{
	RTC.CNT = 0;
	RTC.CLKSEL = RTC_CLKSEL_INT32K_gc;
	RTC.CTRLA = RTC_PRESCALER_DIV32_gc | RTC_RTCEN_bm;
}

void trigger_nvm_cmd(NVMCTRL_CMD_t cmd)
{
	_PROTECTED_WRITE_SPM(NVMCTRL.CTRLA, cmd);
	while((NVMCTRL.STATUS & (NVMCTRL_EEBUSY_bm |  NVMCTRL_FBUSY_bm)));	
}

void load_settings()
{
	//settings map in flash
	//byte 1 = flag for settings saved: 0xff = false, 0x00 = true
	//byte 2 = game speed
	//byte 3 = game mode
	//byte 4 = brightness level
	//byte 5 = game loop mode
	//byte 6 = last slot loaded
	//byte 7 = saved slot flags
	
	volatile uint8_t * address = SETTINGS_START;
	volatile temp = address[0];
	if (temp == 0)
	{
		game_speed = address[1];
		game_mode = address[2];
		brightness_level = address[3];
		game_loop_mode = address[4];
		game_slot = address[5];
		saved_slots = address[6];
	}
	else
	{
		//defaults
		game_speed = SPEED_5;
		menu_state = GAME_SPEED;
		system_state = GAME;
		game_mode = NORMAL;
		brightness_level = LEVEL_3;
		game_loop_mode = REPEAT;
		saved_slots = 0;
		game_slot = 0;
	}
	
	apply_brightness();
	apply_game_slot(game_slot);
}

void save_settings(uint8_t clear)
{
	//_PROTECTED_WRITE_SPM(NVMCTRL.CTRLA, NVMCTRL_CMD_PAGEBUFCLR_gc);
	//while((NVMCTRL.STATUS & (NVMCTRL_EEBUSY_bm |  NVMCTRL_FBUSY_bm)));
	trigger_nvm_cmd(NVMCTRL_CMD_PAGEBUFCLR_gc);
	
	volatile uint8_t * address = SETTINGS_START;
	
	
	if (clear)
	{
		address[0] = 0xFF;
	}
	else
	{
		//because this is a page buffer, it has to be erased before being written to agani
		//so best to keep this logic in an else
		address[0] = 0; //settings are now saved in flash
	}
	
	address[1] = game_speed;
	address[2] = game_mode;
	address[3] = brightness_level;
	address[4] = game_loop_mode;
	address[5] = game_slot;
	address[6] = saved_slots;
//
		//_PROTECTED_WRITE_SPM(NVMCTRL.CTRLA, NVMCTRL_CMD_PAGEERASEWRITE_gc);
		//while((NVMCTRL.STATUS & (NVMCTRL_EEBUSY_bm |  NVMCTRL_FBUSY_bm)));
	trigger_nvm_cmd(NVMCTRL_CMD_PAGEERASEWRITE_gc);
}



void read_slot(uint8_t slot, uint8_t * destBoard)
{
	uint8_t * address = SLOT_START + (slot * 128);

	for (uint8_t i = 0; i < 64; i++)
	{
		destBoard[i] = address[i];
	}
	
}


void save_slot(uint8_t slot, uint8_t * sourceBoard)
{

	uint8_t temp;
	//Slots are 128 bytes long
	uint8_t *slotStart = SLOT_START + (slot * 128);
	uint8_t i;
	

	trigger_nvm_cmd(NVMCTRL_CMD_PAGEBUFCLR_gc);
	for (i = 0; i < 64; i++)
	{

		//Update the page buff with the source board data
		temp = sourceBoard[i];
		//using temp for debugging
		slotStart[i] = temp;

	}

	trigger_nvm_cmd(NVMCTRL_CMD_PAGEERASEWRITE_gc);	
	for (i = 64; i < 128; i++)
	{

		//Update the page buff with the source board data
		temp = sourceBoard[i];
		//using temp for debugging
		slotStart[i] = temp;

	}

	trigger_nvm_cmd(NVMCTRL_CMD_PAGEERASEWRITE_gc);
}

void write_display_section(uint8_t * board, uint8_t address, uint8_t start_index, uint8_t end_index)
{
	uint8_t i;
    if (!(twiAddress(TWI, address, 0)))
    {

	    twiWriteSingle(TWI, CMD_W_DISP); //then the command
	    twiWriteSingle(TWI, 0x00); //then the starting ram address (0)
	    
	    for (i = start_index; i < end_index; i+=4)
	    {
		    twiWriteSingle(TWI, board[i]);
		    twiWriteSingle(TWI, board[i+1]);
	    }
    }
    twiStop(TWI);
}

void write_board(uint8_t * board) 
{
    uint8_t i;
    //board is 32x32. Bytes for board arranged as 4x32:
    /*
     *  [b0, b1, b2, b3
     *   b4, b5, b6, b7
     *          .
     *          .
     *          .
     *   b124, b125, b126, b127]
     */
    
    //1x1: addr 0
	write_display_section(board, BASE_ADDR | 0x00, 0, 62);
	
    //1x2: addr 1
	write_display_section(board, BASE_ADDR | 0x02, 2, 64);

    //2x1: addr 2
	write_display_section(board, BASE_ADDR | 0x04, 64, 126);

    //2x2: addr 3
	write_display_section(board, BASE_ADDR | 0x06, 66, 128);
 
}



void draw_char(uint8_t x, uint8_t y, uint8_t chr, uint8_t * board)
{
	uint8_t index = 0; //index in board array
	uint8_t pos = 0; //bit pos in board[index]
	uint8_t chrByte;
	
	//Make sure the x and y leave enough room for the character
	if (x + FONT_W > WIDTH
	|| y + FONT_H > HEIGHT)
	{
		return;
	}
	
	//Then get the starting board index and bit pos
	// we will assume the WIDTH is byte aligned.
	index = ((y * WIDTH) + x) / 8; //overall xy pos / 8 should be the index;
	pos = (x % 8); //  x % 8 should give bit pos. 7 - pos because we will count down from the MSB
	
	if (chr < 0x20 || chr > 0x7F)
	{
		chr = 0x20;
	}
	
	chr -= 0x20;
	
	//load the character bytes
	//set the bits in the board
	for (uint8_t i = 0; i < FONT_W; i++)
	{
		chrByte = pgm_read_byte(font + (chr * FONT_W) + i);
		
		//Cycle through the bits of the font
		for (uint8_t j = 0; j < FONT_H; j++)
		{
			//fonts are rotated CC 90.
			
			//This for loop moves down bytes in the board
			//and up BITS in the current char byte
			

			
			if ((chrByte & (1 << j)))
			{
				//set byte
				board[index + (j * (WIDTH/8))] |=  1 << pos;
			}
			else
			{
				//clear byte
				board[index + (j * (WIDTH/8))] &= ~(1 << pos);
			}
		}
		
		pos++; //Assuming that we are always inc i by only 1.
		
		if (pos > 7)
		{
			//if we reached here, it means we looped around so we need
			//to bump up the offset for the board index
			//should only happen once per wrap around
			pos = 0;
			index++;
		}
		
		
	}
	

	

}

void draw_string(uint8_t x, uint8_t y, uint8_t * string, uint8_t * board)
{
	uint8_t i = 0;
	uint8_t cursor = x;
	while (string[i] != '\0')
	{
		draw_char(cursor, y, string[i], board);
		i++;
		cursor += FONT_W;
	}

}

void set_board(uint8_t pattern, uint8_t * board)
{
	for (uint8_t i = 0; i < 128; i++)
	{
		board[i] = pattern;
	}
	
}




void draw_menu()
{
	uint8_t numChar[] = 
	{
		'0',
		'1',
		'2',
		'3',
		'4',
		'5',
		'6',
		'7',
		'8',
		'9',
	};
	
	set_board(0x00, sourceBoard);
	
	switch(menu_state)
	{
		case GAME_SPEED:
			draw_string(0, 0, "Game", sourceBoard);
			draw_string(0, 6, "Speed:", sourceBoard);
			draw_char(0, 12, numChar[game_speed], sourceBoard);
			break;
		case BRIGHTNESS:
			draw_string(0, 0, "Light", sourceBoard);
			draw_string(0, 6, "Level:", sourceBoard);
			draw_char(0, 12, numChar[brightness_level], sourceBoard);
			break;
		case NYAN_CAT_MODE:
			draw_string(0, 0, "Nyan", sourceBoard);
			draw_string(0, 6, "Cat:", sourceBoard);
			if (game_mode == NORMAL)
			{
				draw_string(0, 12, "Normal", sourceBoard);
			}
			else
			{
				draw_string(0, 12, "Nyan", sourceBoard);
				draw_string(0, 18, "Cat", sourceBoard);
			}
			break;
		case LOOP_MODE:
			draw_string(0, 0, "Loop", sourceBoard);
			draw_string(0, 6, "Mode:", sourceBoard);
			if (game_loop_mode == REPEAT)
			{
				draw_string(0, 12, "Repeat", sourceBoard);
				draw_string(0, 18, "on", sourceBoard);
				draw_string(0, 24, "Stall", sourceBoard);								
			}
			else
			{
				draw_string(0, 12, "Stop", sourceBoard);
			}			
			break;
		case SLOT_SELECT:
			draw_string(0, 0, "Select", sourceBoard);
			draw_string(0, 6, "Slot:", sourceBoard);
			draw_char(0, 12, numChar[game_slot], sourceBoard);
			break;
		case SLOT_EDIT:
			draw_string(0, 0, "Edit", sourceBoard);
			draw_string(0, 6, "Slot:", sourceBoard);
			draw_char(0, 12, numChar[edit_slot], sourceBoard);
			break;
		case SLOT_SAVE:
			draw_string(0, 0, "Save", sourceBoard);
			draw_string(0, 6, "Slot", sourceBoard);
			draw_char(21, 6, numChar[edit_slot], sourceBoard);
			draw_char(26, 6, ':', sourceBoard);
			switch (slot_save_menu_option)
			{
				case BACK:
					draw_string(0, 12, "Back", sourceBoard);
					break;
				case SAVE:
					draw_string(0, 12, "Save", sourceBoard);
					break;
				case CANCEL:
					draw_string(0, 12, "Cancel", sourceBoard);
					break;
				case RESET:
				default:
					draw_string(0, 12, "Reset", sourceBoard);
					break;
			}
	}
	
	write_board(sourceBoard);

}

void apply_game_slot(uint8_t slot)
{
	sourceBoard = board_0;
	targetBoard = board_1;
	for(uint8_t i = 0; i < 128; i++)
	{
		if ((saved_slots & (1<<slot)))
		{
			read_slot(slot, sourceBoard);
		}
		{
			//load game from first 4 default slots
			sourceBoard[i] = pgm_read_byte(patterns[slot & 0x03] + i);
		}
			
	}
	
	write_board(sourceBoard);
	game_speed_counter = 0;
}

void apply_brightness()
{
	init_display(ALL_ADDR, brightness_level_tabel[brightness_level]);
	_delay_ms(2);
	write_board(sourceBoard);
}

void handle_slot_save_selection(slot_save_menu_options_enum selection)
{
	uint8_t i = 0;

	switch(selection)
	{
		case SAVE:
			save_slot(edit_slot, targetBoard); //targetBoard should be filled with actall pattern data
			saved_slots |= (1<<edit_slot);
			save_settings(0);
			apply_game_slot(game_slot);
			system_state = GAME;
			menu_state = GAME_SPEED;
			break;
		case BACK:
			//get the board back
			for (i = 0; i < 128; i++)
			{
				sourceBoard[i] = targetBoard[i];
			}
			system_state = EDIT;
			break;
		case CANCEL:
			apply_game_slot(game_slot);
			system_state = GAME;			
			menu_state = GAME_SPEED;
			break;
		case RESET:
			saved_slots &= ~(1<<edit_slot);
			save_settings(0);
			apply_game_slot(game_slot);
			system_state = GAME;
			menu_state = GAME_SPEED;
			break;
	}

		
}

void handle_menu_command(command_enum command)
{
	switch(command)
	{
		case RIGHT_CLICK:
			
			if (menu_state < SLOT_EDIT)
			{
				menu_state++;
			}
			
			draw_menu();
			break;
				
		case LEFT_CLICK:

			if (menu_state > GAME_SPEED)
			{
				menu_state--;
			}
			draw_menu();
			break;		
		
		case UP_CLICK:

			switch(menu_state)
			{
				case GAME_SPEED:
					if (game_speed < SPEED_8)
					{
						game_speed++;
					}
					break;
				case BRIGHTNESS:
					if (brightness_level < LEVEL_8)
					{
						brightness_level++;
					}
					apply_brightness();
					break;
				case NYAN_CAT_MODE:
					if (game_mode == NORMAL)
					{
						game_mode = NYAN_CAT;
					}
					else
					{
						game_mode = NORMAL;
					}
					break;
				case LOOP_MODE:
					if (game_loop_mode == REPEAT)
					{
						game_loop_mode = STOP;
					}
					else
					{
						game_loop_mode = REPEAT;
					}
					break;
				case SLOT_SELECT:
					if (game_slot < MAX_SLOTS - 1)
					{
						game_slot++;
					}
					break;
				case SLOT_EDIT:
					if (edit_slot < MAX_SLOTS - 1)
					{
						edit_slot++;
					}
					break;
				case SLOT_SAVE:
					if (slot_save_menu_option > SAVE)
					{
						slot_save_menu_option--;
					}
					break;
					
			}
			
			draw_menu();
			break;
		
		case DOWN_CLICK:
		
			switch(menu_state)
			{
				case GAME_SPEED:
					if (game_speed > SPEED_1)
					{
						game_speed--;
					}
					break;
				case BRIGHTNESS:
					if (brightness_level > LEVEL_1)
					{
						brightness_level--;
					}
					apply_brightness();
					break;
				case NYAN_CAT_MODE:
					if (game_mode == NORMAL)
					{
						game_mode = NYAN_CAT;
					}
					else
					{
						game_mode = NORMAL;
					}
					break;
				case LOOP_MODE:
					if (game_loop_mode == REPEAT)
					{
						game_loop_mode = STOP;
					}
					else
					{
						game_loop_mode = REPEAT;
					}
					break;
				case SLOT_SELECT:
					if (game_slot > 0)
					{
						game_slot--;
					}
					break;
				case SLOT_EDIT:
					if (edit_slot > 0)
					{
						edit_slot--;
					}
					break;
				case SLOT_SAVE:
					if (slot_save_menu_option < RESET)
					{
						slot_save_menu_option++;
					}
					break;				
			}
			
			draw_menu();
			break;
			
		case SHORT_SELECT:
		case LONG_SELECT:
		default:
			if (menu_state == SLOT_EDIT)
			{
				apply_game_slot(edit_slot);
				system_state = EDIT;
			}
			else if (menu_state == SLOT_SAVE)
			{
				handle_slot_save_selection(slot_save_menu_option);
			}
			else
			{
				system_state = GAME;
				save_settings(0);
				apply_game_slot(game_slot);
			}
			break;
	}	
}

void handle_in_game_command(command_enum command)
{
	switch (command)	
	{
		
		case UP_CLICK:
			if (brightness_level < LEVEL_8)
			{
				brightness_level++;
			}
			apply_brightness();
			save_settings(0);
			break;
		case DOWN_CLICK:
			if (brightness_level > LEVEL_1)
			{
				brightness_level--;
			}
			apply_brightness();
			save_settings(0);
			break;
		case RIGHT_CLICK:
			if (game_speed < SPEED_8)
			{
				game_speed++;
			}
			save_settings(0);
			break;
		case LEFT_CLICK:
			if (game_speed > SPEED_1)
			{
				game_speed--;
			}
			save_settings(0);
			break;			
		case LONG_SELECT:
			system_state = MENU;
			draw_menu();
			break;
		case SHORT_SELECT:
			if (game_mode == NORMAL)
			{				
				game_slot++;
				if (game_slot >= MAX_SLOTS)
				{
					game_slot = 0;
				}
				//so we can automatically select the current board to edit
				edit_slot = game_slot;
				save_settings(0);
				apply_game_slot(game_slot);
			}
			break;			
	}
}

void handle_edit_command(command_enum command)
{
	uint8_t bit;
	uint8_t index;
	
	switch (command)	
	{
		
		case UP_CLICK:
			if (edit_cursor_y > 0)
			{
				edit_cursor_y--;
			}
			break;
		case DOWN_CLICK:
			if (edit_cursor_y < HEIGHT - 1)
			{
				edit_cursor_y++;
			}
			break;
		case RIGHT_CLICK:
			if (edit_cursor_x < WIDTH - 1)
			{
				edit_cursor_x++;
			}
			break;
		case LEFT_CLICK:
			if (edit_cursor_x > 0)
			{
				edit_cursor_x--;
			}
			break;			
		case LONG_SELECT:
			//Gotta make sure we don't loose anything
			//The menu text will overwrite what is in sourceBoard
			for (uint8_t i = 0; i < 128; i++)
			{
				targetBoard[i] = sourceBoard[i];
			}
			menu_state = SLOT_SAVE;
			system_state = MENU;
			draw_menu();
			break;
		case SHORT_SELECT:
			bit = edit_cursor_x % 8;
			index = (edit_cursor_y * WIDTH_BYTES) + (edit_cursor_x / 8);
			
			//Toggle the bit
			sourceBoard[index] ^= (1 << bit);
			
			break;			
	}	
	
}

void handle_command(command_enum command)
{
	
	switch(system_state)
	{
		case GAME:
			handle_in_game_command(command);
			break;
		case MENU:
			handle_menu_command(command);
			break;
		case EDIT:
			handle_edit_command(command);		
			break;
	}
	
	
}

uint8_t run_game(uint8_t * sourceBoard, uint8_t * targetBoard)
{
	//TODO: Update game stall (deadBoard)
    uint16_t b;
    uint16_t line;
  
    uint8_t stalled = 1;
	uint16_t i = 0;
    
    for (; i < 1024; i++)
    {
        neighbors = 0;

        
        //for top checks
        if (i > 31)
        {
            //set to line above
            line = i - 32;
                    
            //for left side check
            if ((i % 32) > 0)
            {
                b = line - 1;//bit to the left
                
                if ((sourceBoard[b/8] & (1 << (b % 8))))
                {
                    neighbors++;
                }
            }
            
            //center check
            b = line;

            if ((sourceBoard[b/8] & (1 << (b % 8))))
            {
                neighbors++;
            }
                
                
            //for right side check
            if ((i % 32) < 31)
            {
                b = line + 1;//bit to the right
                
                if ((sourceBoard[b/8] & (1 << (b % 8))))
                {
                    neighbors++;
                }                
            
            }
        }
        
        //Middle checks
        line = i;
        //for left side check
        if ((i % 32) > 0)
        {
            b = line - 1;//bit to the left

            if ((sourceBoard[b/8] & (1 << (b % 8))))
            {
                neighbors++;
            }
        }        
        
        //for right side check
        if ((i % 32) < 31)
        {
            b = line + 1;//bit to the right

            if ((sourceBoard[b/8] & (1 << (b % 8))))
            {
                neighbors++;
            }                

        }               
        
        //for bottom checks
        if (i < 1024 - 32)
        {
            //set to line above
            line = i + 32;
            
            //for left side check
            if ((i % 32) > 0)
            {
                b = line - 1;//bit to the left
                
                if ((sourceBoard[b/8] & (1 << (b % 8))))
                {
                    neighbors++;
                }
            }
            
            //center check
            b = line;

            if ((sourceBoard[b/8] & (1 << (b % 8))))
            {
                neighbors++;
            }
                
                
            //for right side check
            if ((i % 32) < 31)
            {
                b = line + 1;//bit to the right
                
                if ((sourceBoard[b/8] & (1 << (b % 8))))
                {
                    neighbors++;
                }                
            
            }            
        }
        
        //mid checks
        
 
        
        //Now decide the fate of the cell
        if (neighbors == CELL_BORN)
			//&& !(targetBoard [i/8] & (1 << (i %8)))) //check to see if the cell is already alive
        {

			if (!(targetBoard [i/8] & (1 << (i %8))))
			{
				stalled = 0; // no longer stalled
			}
						
            //Set the cell to alive no matter what
            targetBoard[i/8] |= (1 << (i % 8));   	

        }
        else if ((neighbors < CELL_DIE_MIN
            || neighbors > CELL_DIE_MAX))
			//&& (targetBoard [i/8] & (1 << (i %8)))) //check to see if the cell is already dead
        {

			if ((targetBoard [i/8] & (1 << (i %8))))
			{
				stalled = 0; // no longer stalled	
			}
			
			//Terminate the cell no matter what
			targetBoard[i/8] &= ~(1 << (i % 8));
        }
        else
        {
            //need to force an update on the target board to match the cell from the source board
            if ((sourceBoard[i/8] & (1 << (i % 8))))
				//&& !(targetBoard [i/8] & (1 << (i %8)))) //check to see if the cell is already alive)
            {
                targetBoard[i/8] |= (1 << (i %8));
				//stalled = 0; // no longer a dead board
			
            }
            else //if ((targetBoard [i/8] & (1 << (i %8)))) //check to see if the cell is already dead
            {
                targetBoard[i/8] &= ~(1 << (i % 8));    
				//stalled = 0; // no longer a dead board       
            }
            
            
        }
        
        
    }
    
    return stalled;
}

void run_nyan_cat()
{
	for(uint8_t i = 0; i < 128; i++)
	{
		sourceBoard[i] = pgm_read_byte(nyan[nyan_cat_index] + i);
	}
	
	write_board(sourceBoard);
	
	nyan_cat_index++;
	
	if (nyan_cat_index >= NYAN_CAT_SLIDES)
	{
		nyan_cat_index = 0;		
	}
	
}

void implement_edit_cursor_markers(uint8_t highlight, uint8_t index, uint8_t bit)
{
	if (highlight)
	{
		targetBoard[index] |= (1 << bit);
	}
	else
	{
		targetBoard[index] &= ~(1 << bit);
	}
	
	
}

void draw_edit_cursor()
{
	uint8_t bit;
	uint8_t index;
	//uint8_t x = edit_cursor_x;
	//uint8_t y = edit_cursor_y;
	//sourceBoard is used for pattern
	//targetBoard is used for cursor overlay
	
	//Cursor marker:
	//     |
	//    - -
	//	   |
	
	//reset the targetBoard;
	for (index = 0; index < 128; index++)
	{
		targetBoard[index] = sourceBoard[index];
	}
	
	index = edit_cursor_y * WIDTH_BYTES + (edit_cursor_x / 8);
	bit = (edit_cursor_x % 8);
		
	if (edit_cursor_x > 0)	//only draw cursor on left side of x > 0
	{			
		if ((edit_cursor_x % 8) == 0) // We are at a byte boundary
		{
			implement_edit_cursor_markers(cursor_highlight, index - 1, 7); //Move to end of previous byte	
		}
		else
		{
			implement_edit_cursor_markers(cursor_highlight, index, bit - 1);//-1 to draw to the left
				
		}
			
		
	}		
	if (edit_cursor_x < WIDTH - 1) //-1 because 0 index. Only draw cursor right marker if we are not on the edge
	{
		if ((edit_cursor_x % 8) == 7) // We are at a byte boundary
		{
			implement_edit_cursor_markers(cursor_highlight, index + 1, 0); // move to start of next byte
		}
		else
		{
			implement_edit_cursor_markers(cursor_highlight, index, bit + 1); //+1 to draw to the right			
		}
	}

	
	
	if (edit_cursor_y > 0) //Only draw the top marker if we are not at the top edge
	{
		implement_edit_cursor_markers(cursor_highlight, index - WIDTH_BYTES, bit); //-WIDTH_BYTES to draw one above		
	}
	if (edit_cursor_y < HEIGHT - 1) //Only draw the top marker if we are not at the bottom edge
	{
		implement_edit_cursor_markers(cursor_highlight, index + WIDTH_BYTES, bit); //+WIDTH_BYTES to draw one below
	}		
		
	cursor_highlight = ~cursor_highlight;
	
	write_board(targetBoard);
}

int main(void) {
    

	sourceBoard = board_0;

	PORTA.DIRCLR = ALL_BUTTONS_bm;
	PORTA.PIN7CTRL = PORT_PULLUPEN_bm | PORT_INVEN_bm;
	PORTA.PIN6CTRL = PORT_PULLUPEN_bm | PORT_INVEN_bm;
	PORTA.PIN5CTRL = PORT_PULLUPEN_bm | PORT_INVEN_bm;
	PORTA.PIN4CTRL = PORT_PULLUPEN_bm | PORT_INVEN_bm;
	PORTA.PIN3CTRL = PORT_PULLUPEN_bm | PORT_INVEN_bm;




	init_clock();
	init_rtc_clock();
	twiInitialize(TWI);

	if ((PORTA.IN & DIR_BUTTONS_bm) == DIR_BUTTONS_bm)
	{
		save_settings(1);//save with clear flag set
	}
		
	_delay_ms(2);

	init_display(ALL_ADDR, brightness_level_tabel[2]);

	_delay_ms(2);

	set_board(0xFF, sourceBoard);

	
	write_board(sourceBoard);
	
	_delay_ms(1000);
	
	set_board(0x00, sourceBoard);

	write_board(sourceBoard);
	
	load_settings();
	
	/* Replace with your application code */
	while (1)
	{
		if (RTC.CNT >= BASE_COUNT)
		{
			RTC.CNT = 0;
			game_speed_counter++;
		
		
			if ((system_state == GAME && game_speed_counter >= game_speed_table[game_speed])
				|| (system_state == EDIT && game_speed_counter >= EDIT_CURSOR_SPEED))
			{
				game_speed_counter = 0;
			
				if (system_state == GAME)
				{
					if (game_mode == NORMAL)
					{
						if (run_game(sourceBoard, targetBoard)
							&& game_loop_mode == REPEAT)
						{
							//game had stalled and mode is repeat
							apply_game_slot(game_slot);
						}
						else
						{
							
						
							//Do board swap
							if (targetBoard == board_0)
							{
								targetBoard = board_1;
								sourceBoard = board_0;
							}
							else
							{
								targetBoard = board_0;
								sourceBoard = board_1;
							}
						
							write_board(sourceBoard);
						}
					}
					else
					{
						run_nyan_cat();					
					}
				}
				else if (system_state == EDIT)
				{					
					draw_edit_cursor();
				}
			}
		
			//First detection of a button press
			if (pressed_button == 0
				&& (PORTA.IN & ALL_BUTTONS_bm)
				&& selected_button == 0) // for no-repeate resasons
			{
				pressed_button = PORTA.IN & 0xF8;// ALL_BUTTONS_bm;
				button_debounce_counter	= 0;
				selected_button = 0;
			}
		
			//debouncing a button
			if (pressed_button != 0
				&& pressed_button == (PORTA.IN & ALL_BUTTONS_bm)
				&& selected_button == 0)
			{
				button_debounce_counter++;
			}
		
			//No buttons pressed
			if (!(PORTA.IN & ALL_BUTTONS_bm)
				&& selected_button != 0)
			{
				if (pressed_button == SELECT_BUTTON_bm) //Using pressed_button because selected_button is used for no-repeat
				{
					handle_command(SHORT_SELECT);
				}
			
				pressed_button = 0;
				button_debounce_counter = 0;
				button_long_hold_counter = 0;
				selected_button = 0;
			}
		
			//Button debounced. Promote to pressed
			if (button_debounce_counter >= DEBOUNCE_COUNT
				&& pressed_button != 0)
			{
				selected_button = pressed_button;
			
				//Directional buttons get evaluated on press
				if ((selected_button & (UP_BUTTON_bm | DOWN_BUTTON_bm | LEFT_BUTTON_bm | RIGHT_BUTTON_bm)))
				{
					handle_command(pressed_button);
					pressed_button = 0; //Stop the repeat
				}
			
				if (selected_button == SELECT_BUTTON_bm
				&& button_long_hold_counter > LONG_HOLD_COUNT)
				{
					handle_command(LONG_SELECT);
					pressed_button = 0; //Stop the repeat
				}
			}
		
			//Do count for long hold. Don't roll over
			if (selected_button != 0 
				&& button_long_hold_counter < 255)
			{
				button_long_hold_counter++;
			}
		
		
		}
	
	}
 

    
    return 0;
}
