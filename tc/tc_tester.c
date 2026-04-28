#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>

#include "tc.h"

static uint16_t terminal_size_x;
static uint16_t terminal_size_y;

void sigint_handler(int sig);
void sigwinch_handler(int sig);

int main(void) {
	srand(time(NULL));
	signal(SIGINT, sigint_handler);
	signal(SIGWINCH, sigwinch_handler);

	tc_init();
	tc_set_echo_mode(false);
	tc_set_canonical_mode(false);
	tc_hide_cursor(true);

	tc_get_terminal_size(&terminal_size_y, &terminal_size_x);

	tc_erase_to_origin();
	fflush(stdout);

	tc_text_font_d font1 = tc_get_present_text_font();
	tc_text_font_d font2 = font1;
	
	font1.fg_color = TC_RED;
	font2.fg_color = TC_GREEN;

	font1.strikethrough = true;

	tc_set_text_font(&font1);
	printf("hello\n");

	tc_set_text_font(&font2);
	printf("hello\n");

	tc_set_text_font(&font1);
	printf("hello\n");

	tc_set_text_font(&font2);
	printf("hello\n");

	tc_set_text_font(&font1);
	printf("hello\n");

	tc_set_text_font(&font2);
	printf("hello\n");

	tc_hide_cursor(false);
	tc_reset_font();
	putchar('\n');
	tc_restore_defaults();
	return 0;
}

void sigint_handler(int sig) {
	tc_reset_font();
	tc_erase_to_origin();
	tc_hide_cursor(false);
	printf("termined with SIGINT\n");
	tc_restore_defaults();
	exit(0);
}

void sigwinch_handler(int sig) {
	uint16_t row, col;
	tc_get_terminal_size(&row, &col);
	terminal_size_x = col;
	terminal_size_y = row;
}
