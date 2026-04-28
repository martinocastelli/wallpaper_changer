#ifndef __TC_H__
#define __TC_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

enum {
	STDIN_BUFF_SIZE = 4096,
};

typedef enum {
	TC_BLACK = 0,
	TC_RED = 1,
	TC_GREEN = 2,
	TC_YELLOW = 3,
	TC_BLUE = 4,
	TC_MAGENTA = 5,
	TC_CYAN = 6,
	TC_WHITE = 7,
	TC_GRAY = 8,

	TC_BRIGHT_RED = 9,
	TC_BRIGHT_GREEN = 10,
	TC_BRIGHT_YELLOW = 11,
	TC_BRIGHT_BLUE = 12,
	TC_BRIGHT_MAGENTA = 13,
	TC_BRIGHT_CYAN = 14,
	TC_BRIGHT_WHITE = 15,

	TC_COLOR_DEFAULT,
	TC_COLOR_NONE,
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

typedef struct {
	tc_standard_color_e fg_color;
	tc_standard_color_e bg_color;
	bool bold;
	bool underline;
	bool blinking;
	bool strikethrough;
} tc_text_font_d;

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

// void tc_read_cursor_pos(uint16_t *line, uint16_t *column); //ToDo (maybie not)

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
void tc_set_bg_color_default(void);
void tc_set_bg_color_standard(tc_standard_color_e color);
void tc_set_bg_color_6x6x6(uint8_t r, uint8_t g, uint8_t b);
void tc_set_bg_color_24bit(uint8_t r, uint8_t g, uint8_t b);
//set a text font
void tc_set_text_font(const tc_text_font_d *text_font);
tc_text_font_d tc_get_present_text_font(void);

void tc_get_terminal_size(uint16_t* rows, uint16_t* colmuns);
void tc_set_echo_mode(bool echo);
void tc_set_canonical_mode(bool canon);
void tc_empty_input_buffer(void);
void tc_read_raw_input_buffer(char* str); // max size is 4096 
void tc_get_pressed_keys(tc_keyboard_input_d* keyboard_input); 

#endif //__TC_H__ 
