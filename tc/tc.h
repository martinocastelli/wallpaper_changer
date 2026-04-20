#ifndef __TC_H__
#define __TC_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

enum {
	STDIN_BUFF_SIZE = 4096
};

typedef enum {
	BLACK = 0,
	RED = 1,
	GREEN = 2,
	YELLOW = 3,
	BLUE = 4,
	MAGENTA = 5,
	CYAN = 6,
	WHITE = 7,
	GRAY = 8,
	
	BRIGHT_RED = 9,
	BRIGHT_GREEN = 10,
	BRIGHT_YELLOW = 11,
	BRIGHT_BLUE = 12,
	BRIGHT_MAGENTA = 13,
	BRIGHT_CYAN = 14,
	BRIGHT_WHITE = 15,

	TC_STANDARD_COLOR_MAX,
	TC_STANDARD_COLOR_FIRST = BLACK
} tc_standard_color_e;

typedef enum {
	TC_ARROW_UP,
	TC_ARROW_DOWN,
	TC_ARROW_RIGHT,
	TC_ARROW_LEFT 
} tc_escape_sequence_input_e;

typedef struct {
	tc_escape_sequence_input_e* escape_input;
	size_t escape_input_size;
	char* normal_input;
	size_t normal_input_size;
} tc_keyboard_input_d;

//init terminal attr
void tc_init(void);
void tc_restore_defaults(void);

//move cursor
void tc_move_up(uint16_t n);
void tc_move_down(uint16_t n);
void tc_move_right(uint16_t n);
void tc_move_left(uint16_t n);
void tc_move_to(uint16_t row, uint16_t column);
void tc_move_oigin(void);
void tc_save_cursor_pos(void); //return value: true=saved false=a position was already saved
void tc_restore_cursor_pos(void); //return value: true=a position was saved false=no position where saved
void tc_hide_cursor(bool hide_Nshow);

// void tc_read_cursor_pos(uint16_t *line, uint16_t *column); //TODO (maybie not)

//erase_display
void tc_erase_screen(void);
void tc_erase_to_origin(void); //erase all screen and move to origin (1, 1)
void tc_erase_line(void);
void tc_erase_all_before(void);
void tc_erase_all_after(void);

//font manipolation
void tc_reset_font(void);
void tc_set_bold(bool set_Nset);
void tc_set_underline(bool set_Nset);
void tc_set_blinking(bool set_Nset);
void tc_set_strikethrough(bool set_Nset);
//color
void tc_set_color_default(void);
void tc_set_color_standard(tc_standard_color_e color);
void tc_set_color_6x6x6(uint8_t r, uint8_t g, uint8_t b);
void tc_set_color_24bit(uint8_t r, uint8_t g, uint8_t b);
void tc_set_bg_color_standard(tc_standard_color_e color);
void tc_set_bg_color_6x6x6(uint8_t r, uint8_t g, uint8_t b);
void tc_set_bg_color_24bit(uint8_t r, uint8_t g, uint8_t b);

void tc_get_terminal_size(uint16_t* rows, uint16_t* colmuns);
void tc_set_echo_mode(bool echo);
void tc_set_canonical_mode(bool canon);
void tc_empty_input_buffer(void);
void tc_read_raw_input_buffer(char* str); // max size is 4096 
void tc_get_pressed_keys(tc_keyboard_input_d* keyboard_input); 

#endif //__TC_H__ 
