#include <avr/pgmspace.h> 


const uint8_t font[] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, // ''
	0x00, 0x00, 0x17, 0x00, 0x00, // '!'
	0x00, 0x03, 0x00, 0x03, 0x00, // '"'
	0x0a, 0x1f, 0x0a, 0x1f, 0x0a, // '#'
	0x12, 0x15, 0x1f, 0x15, 0x09, // '$'
	0x13, 0x0b, 0x04, 0x1a, 0x19, // '%'
	0x0d, 0x12, 0x15, 0x08, 0x14, // '&'
	0x00, 0x00, 0x03, 0x00, 0x00, // '''
	0x00, 0x0e, 0x11, 0x00, 0x00, // '('
	0x00, 0x00, 0x11, 0x0e, 0x00, // ')'
	0x00, 0x05, 0x02, 0x05, 0x00, // '*'
	0x04, 0x04, 0x1f, 0x04, 0x04, // '+'
	0x00, 0x10, 0x08, 0x00, 0x00, // ','
	0x04, 0x04, 0x04, 0x04, 0x04, // '-'
	0x00, 0x18, 0x18, 0x00, 0x00, // '.'
	0x10, 0x08, 0x04, 0x02, 0x01, // '/'
	0x0e, 0x11, 0x15, 0x11, 0x0e, // '0'
	0x00, 0x12, 0x1f, 0x10, 0x00, // '1'
	0x00, 0x1d, 0x15, 0x15, 0x17, // '2'
	0x00, 0x11, 0x15, 0x15, 0x0e, // '3'
	0x00, 0x07, 0x04, 0x04, 0x1f, // '4'
	0x00, 0x17, 0x15, 0x15, 0x09, // '5'
	0x00, 0x1f, 0x15, 0x15, 0x1d, // '6'
	0x00, 0x03, 0x19, 0x05, 0x03, // '7'
	0x00, 0x0a, 0x15, 0x15, 0x0a, // '8'
	0x00, 0x02, 0x15, 0x15, 0x0e, // '9'
	0x00, 0x00, 0x12, 0x00, 0x00, // ':'
	0x00, 0x08, 0x1a, 0x00, 0x00, // ';'
	0x00, 0x04, 0x0a, 0x11, 0x00, // '<'
	0x00, 0x0a, 0x0a, 0x0a, 0x0a, // '='
	0x00, 0x11, 0x0a, 0x04, 0x00, // '>'
	0x00, 0x01, 0x15, 0x05, 0x02, // '?'
	0x0e, 0x11, 0x15, 0x05, 0x06, // '@'
	0x00, 0x1f, 0x05, 0x05, 0x1f, // 'A'
	0x00, 0x1f, 0x15, 0x15, 0x0e, // 'B'
	0x00, 0x0e, 0x11, 0x11, 0x0a, // 'C'
	0x00, 0x1f, 0x11, 0x11, 0x0e, // 'D'
	0x00, 0x1f, 0x15, 0x15, 0x11, // 'E'
	0x00, 0x1f, 0x05, 0x05, 0x01, // 'F'
	0x00, 0x0e, 0x11, 0x15, 0x1d, // 'G'
	0x00, 0x1f, 0x04, 0x04, 0x1f, // 'H'
	0x00, 0x11, 0x1f, 0x11, 0x00, // 'I'
	0x00, 0x18, 0x10, 0x10, 0x1f, // 'J'
	0x00, 0x1f, 0x04, 0x0a, 0x11, // 'K'
	0x00, 0x1f, 0x10, 0x10, 0x10, // 'L'
	0x1f, 0x02, 0x0c, 0x02, 0x1f, // 'M'
	0x00, 0x1f, 0x06, 0x0c, 0x1f, // 'N'
	0x00, 0x0e, 0x11, 0x11, 0x0e, // 'O'
	0x00, 0x1f, 0x05, 0x05, 0x02, // 'P'
	0x00, 0x0e, 0x15, 0x19, 0x1e, // 'Q'
	0x00, 0x1f, 0x05, 0x0d, 0x12, // 'R'
	0x00, 0x12, 0x15, 0x15, 0x09, // 'S'
	0x01, 0x01, 0x1f, 0x01, 0x01, // 'T'
	0x00, 0x0f, 0x10, 0x10, 0x0f, // 'U'
	0x03, 0x0c, 0x10, 0x0c, 0x03, // 'V'
	0x0f, 0x10, 0x0c, 0x10, 0x0f, // 'W'
	0x11, 0x0a, 0x04, 0x0a, 0x11, // 'X'
	0x01, 0x02, 0x1c, 0x02, 0x01, // 'Y'
	0x00, 0x19, 0x15, 0x13, 0x11, // 'Z'
	0x00, 0x1f, 0x11, 0x11, 0x00, // '['
	0x01, 0x02, 0x04, 0x08, 0x10, // '\'
	0x00, 0x11, 0x11, 0x1f, 0x00, // ']'
	0x00, 0x02, 0x01, 0x02, 0x00, // '^'
	0x10, 0x10, 0x10, 0x10, 0x10, // '_'
	0x00, 0x01, 0x02, 0x00, 0x00, // '`'
	0x00, 0x08, 0x1a, 0x1a, 0x1c, // 'a'
	0x00, 0x1f, 0x14, 0x14, 0x08, // 'b'
	0x00, 0x0c, 0x12, 0x12, 0x12, // 'c'
	0x00, 0x08, 0x14, 0x14, 0x1f, // 'd'
	0x00, 0x0c, 0x16, 0x16, 0x14, // 'e'
	0x00, 0x04, 0x1e, 0x05, 0x01, // 'f'
	0x00, 0x0c, 0x12, 0x1a, 0x08, // 'g'
	0x00, 0x1f, 0x04, 0x04, 0x18, // 'h'
	0x00, 0x14, 0x14, 0x1d, 0x10, // 'i'
	0x00, 0x18, 0x10, 0x10, 0x1d, // 'j'
	0x00, 0x1f, 0x08, 0x0c, 0x10, // 'k'
	0x00, 0x11, 0x1f, 0x10, 0x00, // 'l'
	0x1e, 0x02, 0x1e, 0x02, 0x1c, // 'm'
	0x00, 0x1e, 0x02, 0x02, 0x1c, // 'n'
	0x00, 0x0c, 0x12, 0x12, 0x0c, // 'o'
	0x00, 0x1e, 0x0a, 0x0a, 0x04, // 'p'
	0x00, 0x04, 0x0a, 0x0a, 0x1e, // 'q'
	0x00, 0x1c, 0x02, 0x02, 0x02, // 'r'
	0x00, 0x14, 0x1e, 0x1a, 0x0a, // 's'
	0x00, 0x04, 0x0f, 0x14, 0x10, // 't'
	0x00, 0x0e, 0x10, 0x10, 0x1e, // 'u'
	0x00, 0x06, 0x18, 0x18, 0x06, // 'v'
	0x0e, 0x10, 0x0e, 0x10, 0x0e, // 'w'
	0x00, 0x12, 0x0c, 0x0c, 0x12, // 'x'
	0x00, 0x02, 0x14, 0x1c, 0x02, // 'y'
	0x00, 0x12, 0x1a, 0x16, 0x12, // 'z'
	0x00, 0x04, 0x1f, 0x11, 0x11, // '{'
	0x00, 0x00, 0x1f, 0x00, 0x00, // '|'
	0x00, 0x11, 0x11, 0x1f, 0x04, // '}'
	0x00, 0x01, 0x03, 0x02, 0x02, // '~'
};

//const unsigned char font[96][6] PROGMEM = {
	//{0x00,0x00,0x00,0x00,0x00,0x00}, //  
	//{0x5c,0x00,0x00,0x00,0x00,0x00}, // !
	//{0x06,0x00,0x06,0x00,0x00,0x00}, // "
	//{0x28,0x7c,0x28,0x7c,0x28,0x00}, // #
	//{0x5c,0x54,0xfe,0x54,0x74,0x00}, // $
	//{0x44,0x20,0x10,0x08,0x44,0x00}, // %
	//{0x28,0x54,0x54,0x20,0x50,0x00}, // &
	//{0x06,0x00,0x00,0x00,0x00,0x00}, // '
	//{0x38,0x44,0x00,0x00,0x00,0x00}, // (
	//{0x44,0x38,0x00,0x00,0x00,0x00}, // )
	//{0x02,0x07,0x02,0x00,0x00,0x00}, // *
	//{0x10,0x10,0x7c,0x10,0x10,0x00}, // +
	//{0xc0,0x00,0x00,0x00,0x00,0x00}, // ,
	//{0x10,0x10,0x10,0x10,0x10,0x00}, // -
	//{0x40,0x00,0x00,0x00,0x00,0x00}, // .
	//{0x60,0x10,0x0c,0x00,0x00,0x00}, // /
	//{0x7c,0x64,0x54,0x4c,0x7c,0x00}, // 0
	//{0x48,0x7c,0x40,0x00,0x00,0x00}, // 1
	//{0x64,0x54,0x54,0x54,0x48,0x00}, // 2
	//{0x44,0x54,0x54,0x54,0x6c,0x00}, // 3
	//{0x3c,0x20,0x70,0x20,0x20,0x00}, // 4
	//{0x5c,0x54,0x54,0x54,0x24,0x00}, // 5
	//{0x7c,0x54,0x54,0x54,0x74,0x00}, // 6
	//{0x04,0x04,0x64,0x14,0x0c,0x00}, // 7
	//{0x7c,0x54,0x54,0x54,0x7c,0x00}, // 8
	//{0x5c,0x54,0x54,0x54,0x7c,0x00}, // 9
	//{0x44,0x00,0x00,0x00,0x00,0x00}, // :
	//{0xc4,0x00,0x00,0x00,0x00,0x00}, // ;
	//{0x10,0x28,0x44,0x00,0x00,0x00}, // <
	//{0x28,0x28,0x28,0x28,0x28,0x00}, // =
	//{0x44,0x28,0x10,0x00,0x00,0x00}, // >
	//{0x08,0x04,0x54,0x08,0x00,0x00}, // ?
	//{0x7c,0x44,0x54,0x54,0x5c,0x00}, // @
	//{0x7c,0x24,0x24,0x24,0x7c,0x00}, // A
	//{0x7c,0x54,0x54,0x54,0x6c,0x00}, // B
	//{0x7c,0x44,0x44,0x44,0x44,0x00}, // C
	//{0x7c,0x44,0x44,0x44,0x38,0x00}, // D
	//{0x7c,0x54,0x54,0x54,0x44,0x00}, // E
	//{0x7c,0x14,0x14,0x14,0x04,0x00}, // F
	//{0x7c,0x44,0x44,0x54,0x74,0x00}, // G
	//{0x7c,0x10,0x10,0x10,0x7c,0x00}, // H
	//{0x44,0x44,0x7c,0x44,0x44,0x00}, // I
	//{0x60,0x40,0x40,0x44,0x7c,0x00}, // J
	//{0x7c,0x10,0x10,0x28,0x44,0x00}, // K
	//{0x7c,0x40,0x40,0x40,0x40,0x00}, // L
	//{0x7c,0x08,0x10,0x08,0x7c,0x00}, // M
	//{0x7c,0x08,0x10,0x20,0x7c,0x00}, // N
	//{0x38,0x44,0x44,0x44,0x38,0x00}, // O
	//{0x7c,0x14,0x14,0x14,0x08,0x00}, // P
	//{0x3c,0x24,0x64,0x24,0x3c,0x00}, // Q
	//{0x7c,0x14,0x14,0x14,0x68,0x00}, // R
	//{0x5c,0x54,0x54,0x54,0x74,0x00}, // S
	//{0x04,0x04,0x7c,0x04,0x04,0x00}, // T
	//{0x7c,0x40,0x40,0x40,0x7c,0x00}, // U
	//{0x0c,0x30,0x40,0x30,0x0c,0x00}, // V
	//{0x3c,0x40,0x30,0x40,0x3c,0x00}, // W
	//{0x44,0x28,0x10,0x28,0x44,0x00}, // X
	//{0x0c,0x10,0x60,0x10,0x0c,0x00}, // Y
	//{0x44,0x64,0x54,0x4c,0x44,0x00}, // Z
	//{0x7c,0x44,0x00,0x00,0x00,0x00}, // [
	//{0x0c,0x10,0x60,0x00,0x00,0x00}, // "\"
	//{0x44,0x7c,0x00,0x00,0x00,0x00}, // ]
	//{0x00,0x01,0x00,0x01,0x00,0x00}, // ^
	//{0x40,0x40,0x40,0x40,0x40,0x40}, // _
	//{0x00,0x01,0x00,0x00,0x00,0x00}, // `
	//{0x7c,0x24,0x24,0x24,0x7c,0x00}, // a
	//{0x7c,0x54,0x54,0x54,0x6c,0x00}, // b
	//{0x7c,0x44,0x44,0x44,0x44,0x00}, // c
	//{0x7c,0x44,0x44,0x44,0x38,0x00}, // d
	//{0x7c,0x54,0x54,0x54,0x44,0x00}, // e
	//{0x7c,0x14,0x14,0x14,0x04,0x00}, // f
	//{0x7c,0x44,0x44,0x54,0x74,0x00}, // g
	//{0x7c,0x10,0x10,0x10,0x7c,0x00}, // h
	//{0x44,0x44,0x7c,0x44,0x44,0x00}, // i
	//{0x60,0x40,0x40,0x44,0x7c,0x00}, // j
	//{0x7c,0x10,0x10,0x28,0x44,0x00}, // k
	//{0x7c,0x40,0x40,0x40,0x40,0x00}, // l
	//{0x7c,0x08,0x10,0x08,0x7c,0x00}, // m
	//{0x7c,0x08,0x10,0x20,0x7c,0x00}, // n
	//{0x38,0x44,0x44,0x44,0x38,0x00}, // o
	//{0x7c,0x14,0x14,0x14,0x08,0x00}, // p
	//{0x3c,0x24,0x64,0x24,0x3c,0x00}, // q
	//{0x7c,0x14,0x14,0x14,0x68,0x00}, // r
	//{0x5c,0x54,0x54,0x54,0x74,0x00}, // s
	//{0x04,0x04,0x7c,0x04,0x04,0x00}, // t
	//{0x7c,0x40,0x40,0x40,0x7c,0x00}, // u
	//{0x0c,0x30,0x40,0x30,0x0c,0x00}, // v
	//{0x3c,0x40,0x30,0x40,0x3c,0x00}, // w
	//{0x44,0x28,0x10,0x28,0x44,0x00}, // x
	//{0x0c,0x10,0x60,0x10,0x0c,0x00}, // y
	//{0x44,0x64,0x54,0x4c,0x44,0x00}, // z
	//{0x10,0x7c,0x44,0x00,0x00,0x00}, // {
	//{0x6c,0x00,0x00,0x00,0x00,0x00}, // |
	//{0x44,0x7c,0x10,0x00,0x00,0x00}, // }
	//{0x02,0x01,0x02,0x01,0x00,0x00}, // ~
	//{0x00,0x00,0x00,0x00,0x00,0x00}
//};