#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

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
const char command_prefix[] = "awww img -t grow --transition-pos 10,10 --transition-duration 2  ~/Pictures/Wallpapers/";
const char menu_entries[][max_menu_entries_len] = {
	"animated_wallpaper.gif",
	"big_city.gif",
	"big_temple.gif",
	"bird-on-branch.gif",
	"jager.gif",
	"luna_rossa.gif",
	"snowfall-in-forest.gif",
	"technoblade_wallpaper.jpg",
	"wallpaper01.png",
	"wallpaper02.png",
	"wallpaper03.png",
	"wallpaper04.png",
	"wallpaper05.png",
	"wallpaper06.png",
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
		if(actions.enter == true) {
			char buff[max_menu_entries_len + sizeof(command_prefix)/sizeof(char)];
			snprintf(buff, max_menu_entries_len + sizeof(command_prefix)/sizeof(char), "%s%s", command_prefix, menu_entries[state]);
			system(buff);
			actions.enter = false;
		}
		usleep(50000);
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
