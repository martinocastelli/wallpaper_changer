#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ioctl.h>

#include "tc.h"

static struct termios tty_default, tty_state;

void tc_init(void) {
	tcgetattr(STDIN_FILENO, &tty_state);
	tty_default = tty_state;
}
void tc_restore_defaults(void) {
	tcsetattr(STDIN_FILENO, TCSANOW, &tty_default);
}

//move cursor
void tc_move_up(uint16_t n) {
	printf("\033[%uA", n);
}
void tc_move_down(uint16_t n) {
	printf("\033[%uB", n);
}
void tc_move_right(uint16_t n) {
	printf("\033[%uC", n);
}
void tc_move_left(uint16_t n) {
	printf("\033[%uD", n);
}
void tc_move_to(uint16_t row, uint16_t column) {
	printf("\033[%u;%uH", row, column);
}
void tc_move_oigin(void) {
	printf("\033[H");
}
void tc_save_cursor_pos(void) {
	printf("\033[s");
}
void tc_restore_cursor_pos(void) {
	printf("\033[u");
}
void tc_hide_cursor(bool hide_Nshow) {
	printf("\033[?25%c", hide_Nshow?'l':'h');
}

void tc_erase_screen(void) {
	printf("\033[2J");
}
void tc_erase_to_origin(void) {
	printf("\033[H\033[2J");
}
void tc_erase_line(void) {
	printf("\033[2K");
}
void tc_erase_all_before(void) {
	printf("\033[1J");
}
void tc_erase_all_after(void) {
	printf("\033[0J");
}

void tc_reset_font(void) {
	printf("\033[0m");
}
void tc_set_bold(bool set_Nset) {
	printf("\033[%dm", set_Nset?1:22);
}
void tc_set_underline(bool set_Nset) {
	printf("\033[%dm", set_Nset?4:24);
}
void tc_set_blinking(bool set_Nset) {
	printf("\033[%dm", set_Nset?5:25);
}
void tc_set_strikethrough(bool set_Nset) {
	printf("\033[%dm", set_Nset?9:29);
}

void tc_set_color_default(void) {
	printf("\033[39m\033[49m");
}
void tc_set_color_standard(tc_standard_color_e color) {
	printf("\033[38;5;%um", color);
}
void tc_set_color_6x6x6(uint8_t r, uint8_t g, uint8_t b) {
	if (r < 6 && g < 6 && b <6) {
		printf("\033[38;5;%um", 16 + 36 * r + 6 * g + b);
	}
}
void tc_set_color_24bit(uint8_t r, uint8_t g, uint8_t b) {
	printf("\033[38;2;%u;%u;%um", r, g, b);
}
void tc_set_bg_color_standard(tc_standard_color_e color) {
	printf("\033[48;5;%um", color);
}
void tc_set_bg_color_6x6x6(uint8_t r, uint8_t g, uint8_t b) {
	if (r < 6 && g < 6 && b <6) {
		printf("\033[48;5;%um", 16 + 36 * r + 6 * g + b);
	}
}
void tc_set_bg_color_24bit(uint8_t r, uint8_t g, uint8_t b) {
	printf("\033[48;2;%u;%u;%um", r, g, b);
}

void tc_get_terminal_size(uint16_t* rows, uint16_t* columns) {
	struct winsize w;
	ioctl(STDIN_FILENO, TIOCGWINSZ, &w);
	*columns = w.ws_col;
	*rows = w.ws_row;
}
void tc_set_echo_mode(bool echo) {
	echo ? (tty_state.c_lflag |= ECHO) : (tty_state.c_lflag &= ~ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &tty_state);
}
void tc_set_canonical_mode(bool canon) {
	canon ? (tty_state.c_lflag |= ICANON) : (tty_state.c_lflag &= ~ICANON);
	tty_state.c_cc[VMIN] = 0;
	tty_state.c_cc[VTIME] = 0;

	tcsetattr(STDIN_FILENO, TCSANOW, &tty_state);
}
void tc_empty_input_buffer(void) {
	char buff[STDIN_BUFF_SIZE];
	tc_read_raw_input_buffer(buff);
}
void tc_read_raw_input_buffer(char* str) {
	size_t size = read(STDIN_FILENO, str, STDIN_BUFF_SIZE);
	str[size] = '\0';
}

void tc_get_pressed_keys(tc_keyboard_input_d* keyboard_input) {
	char buff[STDIN_BUFF_SIZE];
	tc_read_raw_input_buffer(buff);
	keyboard_input->normal_input_size = 0;
	keyboard_input->escape_input_size = 0;
	for(size_t i = 0;buff[i] != '\0';i++) {
		if (buff[i] != '\033') {
			keyboard_input->normal_input_size++;
		} else {
			keyboard_input->escape_input_size++;
			i += 2;
		}
	}
	free(keyboard_input->normal_input);
	keyboard_input->normal_input = (char*)malloc((keyboard_input->normal_input_size + 1)  * sizeof(char));
	free(keyboard_input->escape_input);
	keyboard_input->escape_input = (tc_escape_sequence_input_e*)malloc(keyboard_input->escape_input_size  * sizeof(tc_escape_sequence_input_e));

	size_t x = 0;
	size_t y = 0;
	for(size_t i = 0;buff[i] != '\0';i++) {
		if (buff[i] != '\033') {
			keyboard_input->normal_input[x++] = buff[i];
		} else if (buff[i + 1] == '[') {
			switch(buff[i + 2]) {
				case 'A': {
					keyboard_input->escape_input[y++] = TC_ARROW_UP;
				} break;
				case 'B': {
					keyboard_input->escape_input[y++] = TC_ARROW_DOWN;
				} break;
				case 'C': {
					keyboard_input->escape_input[y++] = TC_ARROW_RIGHT;
				} break;
				case 'D': {
					keyboard_input->escape_input[y++] = TC_ARROW_LEFT;
				} break;
				default: {
				}
			}
		}
	}
	keyboard_input->normal_input[keyboard_input->normal_input_size] = '\0';
}
