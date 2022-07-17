/*********************
 *      INCLUDES
 *********************/

#include "terminal_bounce.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <time.h>
#include <unistd.h>

/*********************
 *      DEFINES
 *********************/

// 10 fps
#define SLEEP_PERIOD_US (100 * 1000)

#define STEP_SIZE_X 1
#define STEP_SIZE_Y 1

/**********************
 *      TYPEDEFS
 **********************/

struct text_box {
    uint32_t width;
    uint32_t height;
};

struct text_handle {
    struct text_box box;
    struct text_box terminal;
    uint32_t x;
    uint32_t y;
    char* text;
};

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void clear_terminal(void);

static void render_text(struct text_handle* handle);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void terminal_bounce_init(terminal_handle_t* handle, const char* text) {
    *handle = malloc(sizeof(struct text_handle));
    struct text_handle* txt_handle = (struct text_handle*)handle;

    txt_handle->text = malloc(sizeof(char) * strlen(text));
    memcpy(txt_handle->text, text, strlen(text));

    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    txt_handle->terminal.height = w.ws_row;
    txt_handle->terminal.width = w.ws_col;

    txt_handle->box.height = 1;
    txt_handle->box.width = strlen(txt_handle->text);

    srand(time(0));
    txt_handle->x = (rand() % (txt_handle->terminal.width - 1 -
                               txt_handle->box.width - 0 + 1)) +
                    0;
    txt_handle->y = (rand() % (txt_handle->terminal.height - 1 -
                               txt_handle->box.height - 0 + 1)) +
                    0;
}

void terminal_bounce_play(terminal_handle_t* handle) {
    struct text_handle* txt_handle = (struct text_handle*)handle;

    clear_terminal();

    while (1) {
        clear_terminal();

        render_text(txt_handle);

        usleep(SLEEP_PERIOD_US);
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void clear_terminal(void) { printf("\e[1;1H\e[2J"); }

static void render_text(struct text_handle* handle) {
    static int x_direction = STEP_SIZE_X, y_direction = STEP_SIZE_Y;
    handle->x += x_direction;
    handle->y += y_direction;

    for (int i = 0; i < handle->terminal.height; i++) {
        if (i == 0 || i + 1 == handle->terminal.height) {
            for (int k = 0; k < handle->terminal.width; k++) {
                printf("#");
            }
            continue;
        }
        printf("\n");
        for (int j = 0; j < handle->terminal.width; j++) {
            if (i == handle->y && j == handle->x) {
                printf("%s", handle->text);
                j += handle->box.width;
            }
            if (j == 0 || j + 1 == handle->terminal.width) {
                printf("#");
            } else {
                printf(" ");
            }
        }
    }

    if (handle->x + handle->box.width + 1 >= handle->terminal.width) {
        x_direction = -1 * STEP_SIZE_X;
    } else if (handle->x - 1 <= 0) {
        x_direction = STEP_SIZE_X;
    }
    if (handle->y + handle->box.height + 1 >= handle->terminal.height) {
        y_direction = -STEP_SIZE_Y;
    } else if (handle->y - 1 <= 0) {
        y_direction = STEP_SIZE_Y;
    }

    fflush(stdout);
}
