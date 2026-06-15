#include <complex.h>
#include <float.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <glob.h>

#include "tc/tc.h"

void initial_setup(void);

void get_menu_entries_list(bool with_hidden);

void parse_input(void);
void redraw_screen(void);
void quit_program(void);
void print_img(uint16_t x, uint16_t y, uint16_t width);

void sigint_handler(int sig);
void sigwinch_handler(int sig);

const char pictures_path[] = "/home/martino/Pictures/Wallpapers";
const char select_image_command_prefix[] = "awww img -t grow --transition-pos 10,10 --transition-duration 1.5";
const char render_image_command_prefix[] = "kitten icat --align=left --place";
char **menu_entries = NULL;
size_t menu_entries_size = 0;
size_t menu_entries_non_gif_size = 0;
size_t max_menu_entries_len = 0;

static uint8_t selected = 0;
typedef struct {
	bool refresh;
	bool quit;
	bool enter;
	bool move_up;
	bool move_down;
	bool toggle_hidden;
} flags_d;
static flags_d flags;

int main(void) {
	initial_setup();

	get_menu_entries_list(false);

	while(true) {
		parse_input();
		if(flags.quit == true) {
			//just quit like with SIGINT
			quit_program();
		}
		if(flags.move_up == true) {
			selected = selected > 0?selected - 1:menu_entries_size - 1;
			flags.refresh = true;
			flags.move_up = false;
		}
		if(flags.move_down == true) {
			selected = (selected < menu_entries_size - 1)?selected + 1:0;
			flags.refresh = true;
			flags.move_down = false;
		}
		if(flags.toggle_hidden == true) {
			static bool state = false;
			state = !state;
			get_menu_entries_list(state);
			if(selected >= menu_entries_size) {
				selected = menu_entries_size - 1;
			}
			flags.refresh = true;
			flags.toggle_hidden = false;
		}
		if(flags.enter == true) {
			char *buff = (char *)malloc((max_menu_entries_len + sizeof(select_image_command_prefix)/sizeof(char) + 2) * sizeof(char));
			snprintf(buff, max_menu_entries_len + sizeof(select_image_command_prefix)/sizeof(char) + sizeof(pictures_path)/sizeof(char) + 2, "%s %s/%s", select_image_command_prefix, pictures_path, menu_entries[selected]);
			system(buff);
			free(buff);
			flags.enter = false;
		}
		if(flags.refresh == true) {
			redraw_screen();
			flags.refresh = false;
		}
		usleep(50000);
	}
	
	quit_program();
	return 1; // it should not appen
}

void initial_setup(void) {
	signal(SIGINT, sigint_handler);
	signal(SIGWINCH, sigwinch_handler);

	tc_init();
	tc_set_echo_mode(false);
	tc_set_canonical_mode(false);
	tc_hide_cursor(true);

	flags.enter = false;
	flags.quit = false;
	flags.refresh = true;
	flags.toggle_hidden = false;

	tc_erase_to_origin();
	fflush(stdout);
}
void get_menu_entries_list(bool with_hidden) {
	chdir(pictures_path);
	glob_t pglob;
	glob("*.png", 0, NULL, &pglob);
	glob("*.jpg", GLOB_APPEND, NULL, &pglob);
	glob("*.jpeg", GLOB_APPEND, NULL, &pglob);
	menu_entries_non_gif_size = pglob.gl_pathc;
	glob("*.gif", GLOB_APPEND, NULL, &pglob);
	if(with_hidden == true) {
		glob(".*.png", GLOB_APPEND | GLOB_BRACE, NULL, &pglob);
		glob(".*.jpg", GLOB_APPEND | GLOB_BRACE, NULL, &pglob);
		glob(".*.jpeg", GLOB_APPEND | GLOB_BRACE, NULL, &pglob);
		glob(".*.gif", GLOB_APPEND | GLOB_BRACE, NULL, &pglob);
	}

	if(menu_entries != NULL) {
		for(size_t i = 0;i < menu_entries_size;i++) {
			free(menu_entries[i]);
		}
		free(menu_entries);
	}
	menu_entries_size = pglob.gl_pathc;
	menu_entries = (char **)calloc(pglob.gl_pathc, sizeof(char *));
	for(size_t i = 0;i < pglob.gl_pathc;i++) {
		size_t len = strlen(pglob.gl_pathv[i]);
		if (len > max_menu_entries_len) {
			max_menu_entries_len = len;
		}
		printf("%s %lu %lu\n", pglob.gl_pathv[i], len, max_menu_entries_len);
		menu_entries[i] = (char *)malloc(len * sizeof(char));
		strcpy(menu_entries[i], pglob.gl_pathv[i]);
	}
	globfree(&pglob);
}
void parse_input(void) {
	static tc_keyboard_input_d buff;
	tc_get_pressed_keys(&buff);
	for(size_t i = 0;i < buff.normal_input_size;i++) {
		switch(buff.normal_input[i]) {
			case 'K':
			case 'k': {
				flags.move_up = true;
			} break;
			case 'J':
			case 'j': {
				flags.move_down = true;
			} break;
			case '\n': {
				flags.enter = true;
			} break;
			case 'S':
			case 's': {
				flags.toggle_hidden = true;
			} break;
			case 'Q':
			case 'q': {
				flags.quit = true;
			} break;
			default: {
			}
		}
	}
}
void redraw_screen(void) {
	tc_reset_font();
	tc_text_font_d font = tc_get_present_text_font();

	tc_erase_to_origin();

	// title
	tc_set_color_standard(TC_RED);
	tc_set_underline(true);
	tc_set_bold(true);
	printf("Choose a wallpaper\n");
	
	// list
	for(uint16_t i = 0;i < menu_entries_size;i++) {
		if(i == selected) {
			font.fg_color = TC_BLACK;
			font.bg_color = TC_BLUE;
		} else {
			font.fg_color = TC_GREEN;
			font.bg_color = TC_COLOR_DEFAULT;
		}
		tc_set_text_font(&font);
		if(i != 0) {
			putchar('\n');
		}
		printf("%s", menu_entries[i]);
	}
	if(selected < menu_entries_non_gif_size) {
		print_img(max_menu_entries_len + 1, 0, 50);
	}
	fflush(stdout);
}
void quit_program(void) {
	for(size_t i = 0;i < menu_entries_size;i++) {
		free(menu_entries[i]);
	}
	free(menu_entries);
	tc_reset_font();
	tc_erase_to_origin();
	tc_hide_cursor(false);
	tc_restore_defaults();
	exit(0);
}
void print_img(uint16_t x, uint16_t y, uint16_t width) {
	char cmd[1024];
	if(snprintf(cmd, 1024, "%s %ux%u@%ux%u %s", render_image_command_prefix, width, 100, x, y, menu_entries[selected]) > 0) {
		putchar('\n');
		system(cmd);
	}
}

//callbacks
void sigint_handler(int sig) {
	quit_program();
}
void sigwinch_handler(int sig) {
	flags.refresh = true;
}

