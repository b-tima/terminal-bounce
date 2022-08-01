#include <stdio.h>
#include <stdlib.h>

#include "terminal_bounce.h"

static terminal_handle_t term_handle;

int main(int argc, char** argv){
	if(argc < 2){
		exit(EXIT_FAILURE);
	}
	terminal_bounce_init(&term_handle, argv[1]);

	terminal_bounce_play(&term_handle);
	return 0;
}
