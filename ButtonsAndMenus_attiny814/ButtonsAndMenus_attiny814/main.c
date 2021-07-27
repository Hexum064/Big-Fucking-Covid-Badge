/*
 * ButtonsAndMenus_attiny814.c
 *
 * Created: 2021-07-22 22:15:45
 * Author : Branden
 */ 

#include <avr/io.h>
#define  F_CPU 20000000
#include "util/delay.h"
#include "I2CDisp.h"
#include "5x5_font.h"

#define  MAX_SLOTS			8

#define WIDTH				32
#define HEIGHT				32
#define FONT_W				5
#define FONT_H				5

#define BASE_COUNT			10
#define DEBOUNCE_COUNT		5
#define LONG_HOLD_COUNT		200
#define BUTTON_PORT			PORTA
#define DOWN_BUTTON_bm		0b00001000
#define LEFT_BUTTON_bm		0b00010000
#define RIGHT_BUTTON_bm		0b00100000
#define UP_BUTTON_bm		0b01000000
#define SELECT_BUTTON_bm	0b10000000
#define ALL_BUTTONS_bm		DOWN_BUTTON_bm | LEFT_BUTTON_bm | RIGHT_BUTTON_bm | UP_BUTTON_bm | SELECT_BUTTON_bm

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
	GAME_SPEED = 0x00,
	BRIGHTNESS = 0x01,
	NYAN_CAT_MODE = 0x02,
	LOOP_MODE = 0x04,
	SLOT_SELECT = 0x08,
	SLOT_EDIT  = 0x10	
} menu_state_enum;

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

volatile int8_t disp[128];

volatile uint8_t game_speed_counter = 0;
volatile uint8_t button_debounce_counter = 0;
volatile uint8_t button_long_hold_counter = 0;

volatile uint8_t pressed_button = 0;
volatile uint8_t selected_button = 0;

volatile game_speed_enum game_speed;
volatile system_state_enum system_state;
volatile menu_state_enum menu_state;
volatile brightness_level_enum brightness_level;
volatile game_mode_enum game_mode;
volatile game_loop_mode_enum game_loop_mode;
volatile uint8_t game_slot;
volatile uint8_t edit_slot;

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


void write_board(TWI_t *port, uint8_t * board) 
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
    if (!(twiAddress(port, BASE_ADDR | 0x00, 0)))
    {
        twiWriteSingle(port, CMD_W_DISP); //then the command
        twiWriteSingle(port, 0x00); //then the starting ram address (0)
        
        for (i = 0; i < 62; i+=4)
        {
            twiWriteSingle(port, board[i]);
            twiWriteSingle(port, board[i+1]);
        }
    }
    twiStop(port);

    //1x2: addr 1
    if (!(twiAddress(port, BASE_ADDR | 0x02, 0)))
    {
        twiWriteSingle(port, CMD_W_DISP); //then the command
        twiWriteSingle(port, 0x00); //then the starting ram address (0)

        for (i = 2; i < 64; i+=4)
        {
            twiWriteSingle(port, board[i]);
            twiWriteSingle(port, board[i+1]);
        }
    }
    twiStop(port);
    
    //2x1: addr 2
    if (!(twiAddress(port, BASE_ADDR | 0x04, 0)))
    {
        twiWriteSingle(port, CMD_W_DISP); //then the command
        twiWriteSingle(port, 0x00); //then the starting ram address (0)
        
        for (i = 64; i < 126; i+=4)
        {
            twiWriteSingle(port, board[i]);
            twiWriteSingle(port, board[i+1]);
        }
    }
    twiStop(port);

    //2x2: addr 3
    if (!(twiAddress(port, BASE_ADDR | 0x06, 0))) 
    {
        twiWriteSingle(port, CMD_W_DISP); //then the command
        twiWriteSingle(port, 0x00); //then the starting ram address (0)

        for (i = 66; i < 128; i+=4)
        {
            twiWriteSingle(port, board[i]);
            twiWriteSingle(port, board[i+1]);
        }
    }
    twiStop(port);   
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

void clear_board()
{
	for (uint8_t i = 0; i < 128; i++)
	{
		disp[i] = 0;
	}
	
}

void load_settings()
{
	//replace with reads from memory
	game_speed = SPEED_4;
	menu_state = GAME_SPEED;
	system_state = GAME;
	game_mode = NORMAL;	
	brightness_level = LEVEL_3;
	game_loop_mode = REPEAT;
	
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
	
	switch(menu_state)
	{
		case GAME_SPEED:
			draw_string(0, 0, "Game", disp);
			draw_string(0, 6, "Speed", disp);
			draw_char(0, 12, numChar[game_speed], disp);
			break;
		case BRIGHTNESS:
			draw_string(0, 0, "Light", disp);
			draw_string(0, 6, "Level", disp);
			draw_char(0, 12, numChar[brightness_level], disp);
			break;
		case NYAN_CAT_MODE:
			draw_string(0, 0, "Nyan", disp);
			draw_string(0, 6, "Cat", disp);
			if (game_mode == NORMAL)
			{
				draw_string(0, 12, "Normal", disp);
			}
			else
			{
				draw_string(0, 12, "Nyan", disp);
				draw_string(0, 18, "Cat", disp);
			}
			break;
		case LOOP_MODE:
			draw_string(0, 0, "Loop", disp);
			draw_string(0, 6, "Mode", disp);
			if (game_loop_mode == REPEAT)
			{
				draw_string(0, 12, "Repeat", disp);
			}
			else
			{
				draw_string(0, 12, "Stop", disp);
			}			
			break;
		case SLOT_SELECT:
			draw_string(0, 0, "Select", disp);
			draw_string(0, 6, "Slot", disp);
			draw_char(0, 12, numChar[game_slot], disp);
			break;
		case SLOT_EDIT:
			draw_string(0, 0, "Edit", disp);
			draw_string(0, 6, "Slot", disp);
			draw_char(0, 12, numChar[edit_slot], disp);
			break;
	}
	
}

void draw_game()
{
	
}

void save_settings()
{

}

void apply_settings()
{
	
}

void draw_edit()
{
	
}

void handle_menu_command(command_enum command)
{
	clear_board();
	

	if (command == RIGHT_CLICK)
	{
			
		switch(menu_state)
		{
			case GAME_SPEED:
				menu_state = BRIGHTNESS;
				break;
			case BRIGHTNESS:
				menu_state = NYAN_CAT_MODE;
				break;
			case NYAN_CAT_MODE:
				menu_state = LOOP_MODE;
				break;
			case LOOP_MODE:
				menu_state = SLOT_SELECT;
				break;
			case SLOT_SELECT:
				menu_state = SLOT_EDIT;
				break;
			case SLOT_EDIT:
				menu_state = GAME_SPEED;
				break;
		}
			
		draw_menu();
	}
		
	if (command == LEFT_CLICK)
	{
			
			
		switch(menu_state)
		{
			case GAME_SPEED:
				menu_state = SLOT_EDIT;
				break;
			case BRIGHTNESS:
				menu_state = GAME_SPEED;
				break;
			case NYAN_CAT_MODE:
				menu_state = BRIGHTNESS;
				break;
			case LOOP_MODE:
				menu_state = NYAN_CAT_MODE;
				break;
			case SLOT_SELECT:
				menu_state = LOOP_MODE;
				break;
			case SLOT_EDIT:
				menu_state = SLOT_SELECT;
				break;
		}
			
		draw_menu();
	}		
		
	if (command == UP_CLICK)
	{
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
		}
					
		draw_menu();
	}
		
	if (command == DOWN_CLICK)
	{
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
		}
			
		draw_menu();
	}
		
	if (command == SHORT_SELECT | command == LONG_SELECT)		
	{
			
		switch(menu_state)
		{
			if (menu_state == SLOT_EDIT)
			{
				system_state = EDIT;
				draw_edit();
			}
			else
			{
				system_state = GAME;
				save_settings();
				apply_settings();
				draw_game();
			}
								
		}			
	}

	write_board(TWI, disp);
	
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
			save_settings();
			apply_settings();
			break;
		case DOWN_CLICK:
			if (brightness_level > LEVEL_1)
			{
				brightness_level--;
			}
			save_settings();
			apply_settings();
			break;
		case RIGHT_CLICK:
			if (game_speed < SPEED_8)
			{
				game_speed++;
			}
			save_settings();
			apply_settings();
			break;
		case LEFT_CLICK:
			if (game_speed > SPEED_1)
			{
				game_speed--;
			}
			save_settings();
			apply_settings();
			break;			
		case LONG_SELECT:
			system_state = MENU;
			draw_menu();
			write_board(TWI, disp);
			break;
		case SHORT_SELECT:
			if (game_mode = NORMAL)
			{				
				game_slot++;
				if (game_slot >= MAX_SLOTS)
				{
					game_slot = 0;
				}
				//so we can automatically select the current board to edit
				edit_slot = game_slot;
				save_settings();
				apply_settings();
			}
			break;			
	}
}

void handle_edit_command(command_enum command)
{
	
	
}

void handle_command(command_enum command)
{
	
	switch(system_state)
	{
		case GAME:
			handle_in_game_command(command);
		case MENU:
			handle_menu_command(command);
		case EDIT:
			handle_edit_command(command);		
	}
	
	
}

int main(void)
{
	PORTA.DIRCLR = ALL_BUTTONS_bm;
	PORTA.PIN7CTRL = PORT_PULLUPEN_bm | PORT_INVEN_bm;
	PORTA.PIN6CTRL = PORT_PULLUPEN_bm | PORT_INVEN_bm;
	PORTA.PIN5CTRL = PORT_PULLUPEN_bm | PORT_INVEN_bm;
	PORTA.PIN4CTRL = PORT_PULLUPEN_bm | PORT_INVEN_bm;
	PORTA.PIN3CTRL = PORT_PULLUPEN_bm | PORT_INVEN_bm;
	
	init_clock();
	init_rtc_clock();
	twiInitialize(TWI);
	load_settings();
	
	_delay_ms(2);
	
	init_display(ALL_ADDR);
	
	_delay_ms(2);
	
	
	
    /* Replace with your application code */
    while (1) 
    {
		if (RTC.CNT >= BASE_COUNT)
		{
			RTC.CNT = 0;
			game_speed_counter++;
			
			
			if (game_speed_counter >= game_speed)
			{
				game_speed_counter = 0;
				
				//Do game stuff
			}
			
			//First detection of a button press
			if (pressed_button == 0 
				&& (PORTA.IN & 0xF8)//ALL_BUTTONS_bm))
				&& selected_button == 0) // for no-repeate resasons
			{
				pressed_button = PORTA.IN & 0xF8;// ALL_BUTTONS_bm;
				button_debounce_counter	= 0;
				selected_button = 0;	
			}
			
			//debouncing a button
			if (pressed_button != 0
				&& pressed_button == (PORTA.IN & 0xF8)//ALL_BUTTONS_bm)
				&& selected_button == 0)
			{
				button_debounce_counter++;				
			}
			
			//No buttons pressed
			if (!(PORTA.IN & 0xF8)//ALL_BUTTONS_bm))
				&& selected_button != 0)
			{
				if (pressed_button == SELECT_BUTTON_bm) //Using pressed_button because selected_button is used for no-repeat
				{
					//if (button_long_hold_counter > 200)//LONG_HOLD_COUNT)
					//{
						//send_cmd_to_menu(LONG_SELECT);
					//}
					//else
					//{
						handle_command(SHORT_SELECT);
					//}					
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
			if (selected_button != 0 && button_long_hold_counter < 255)
			{
				button_long_hold_counter++;
			}
			
			
		}
		
    }
}

