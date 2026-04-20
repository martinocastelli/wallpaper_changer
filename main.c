#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>

#include "tc/tc.h"

void initial_setup(void);

void parse_input(void);
void redraw_screen(void);
void quit_program(void);

void sigint_handler(int sig);
void sigwinch_handler(int sig);

enum {
	max_menu_entries_len = 64
};
const char menu_entries[][max_menu_entries_len] = {
	"start",
	"start special",
	"about",
	"quit"
};
const size_t menu_entries_size = sizeof(menu_entries) / sizeof(menu_entries[0]);

static uint16_t terminal_size_x;
static uint16_t terminal_size_y;

static uint8_t state = 0;
typedef struct {
	bool refresh;
	bool quit;
	bool enter;
	bool move_up;
	bool move_down;
}actions_d;
static actions_d actions;

int main(void) {
	initial_setup();

	while(true) {
		parse_input();
		if(actions.refresh == true) {
			redraw_screen();
			actions.refresh = false;
		}
		if(actions.quit == true) {
			//just quit like with SIGINT
			quit_program();
		}
		if(actions.enter == true) {
			tc_move_to(terminal_size_y, 1);
			printf("%s", menu_entries[state]);
			fflush(stdout);
			actions.enter = false;
		}
		if(actions.move_up == true) {
			state = state > 0?state - 1:menu_entries_size - 1;
			actions.refresh = true;
			actions.move_up = false;
		}
		if(actions.move_down == true) {
			state = state < menu_entries_size - 1?state + 1:0;
			actions.refresh = true;
			actions.move_down = false;
		}
	}
	
	tc_restore_defaults();
	return 0;
}


void initial_setup(void) {
	signal(SIGINT, sigint_handler);
	signal(SIGWINCH, sigwinch_handler);

	tc_init();
	tc_set_echo_mode(false);
	tc_set_canonical_mode(false);
	tc_hide_cursor(true);

	tc_get_terminal_size(&terminal_size_y, &terminal_size_x);

	actions.enter = false;
	actions.quit = false;
	actions.refresh = true;

	tc_erase_to_origin();
	fflush(stdout);
}
void parse_input(void) {
	static tc_keyboard_input_d buff;
	tc_get_pressed_keys(&buff);
	for(size_t i = 0;i < buff.escape_input_size;i++) {
		switch(buff.escape_input[i]) {
			case TC_ARROW_UP: {
				actions.move_up = true;
			} break;
			case TC_ARROW_DOWN: {
				actions.move_down = true;
			} break;
			default: {
			}
		}
	}
	for(size_t i = 0;i < buff.normal_input_size;i++) {
		switch(buff.normal_input[i]) {
			case '\n': {
				actions.enter = true;
			} break;
			case 'q': {
				actions.quit = true;
			} break;
			default: {
			}
		}
	}
}
void redraw_screen(void) {
	tc_erase_to_origin();
	for(uint16_t i = 0;i < menu_entries_size;i++) {
		if(i == state) {
			tc_set_color_standard(BLACK);
			tc_set_bg_color_standard(BLUE);
		} else {
			tc_set_color_default();
		}
		// printf("scemo in culo %d                         \n", i);
		printf("%s\n", menu_entries[i]);
	}
	fflush(stdout);
	tc_set_color_default();
}
void quit_program(void) {
	tc_reset_font();
	tc_erase_to_origin();
	tc_hide_cursor(false);
	tc_restore_defaults();
	exit(0);
}

//callbacks
void sigint_handler(int sig) {
	quit_program();
}

void sigwinch_handler(int sig) {
	uint16_t row, col;
	tc_get_terminal_size(&row, &col);
	terminal_size_x = col;
	terminal_size_y = row;
}
