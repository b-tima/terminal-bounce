#include <stdio.h>
#include <stdlib.h>

#include "terminal_bounce.h"

static terminal_handle_t term_handle;

int main(){
	terminal_bounce_init(&term_handle, "Hello, Morgan!");

	terminal_bounce_play(&term_handle);
	return 0;
}
