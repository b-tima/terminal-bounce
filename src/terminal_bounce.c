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
#include <signal.h>

/*********************
 *      DEFINES
 *********************/

// 10 fps
#define SLEEP_PERIOD_US (100 * 1000)

#define STEP_SIZE_X 1
#define STEP_SIZE_Y 1

#define NEWLINE_DELIMITER "/--/"

#define WALL_CHAR '#'
#define BACKGROUND_CHAR ' '
#define LINEBREAK_CHAR '\n'

/**********************
 *      TYPEDEFS
 **********************/

struct text_box {
    uint32_t width;
    uint32_t height;
};

struct text_level {
    char* text;
    uint32_t text_len;
};

struct text_handle {
    struct text_box box;
    struct text_box terminal;
    uint32_t x;
    uint32_t y;

    struct text_level* text;
    uint32_t levels;
};

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void sig_handler(int sig);

static struct winsize get_terminal_size(void);

static void clear_terminal(void);

static void hide_cursor(void);

static void show_cursor(void);

static int count_substrings(const char* string, const char* substring);

static void format_text(const char* text, struct text_level** formatted_text,
                        uint32_t* levels);

static void render_text(struct text_handle* handle);

/**********************
 *  STATIC VARIABLES
 **********************/

static volatile int keep_running = 1;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void terminal_bounce_init(terminal_handle_t* handle, const char* text) {
    *handle = malloc(sizeof(struct text_handle));
    struct text_handle* txt_handle = (struct text_handle*)handle;

    format_text(text, &(txt_handle->text), &(txt_handle->levels));

    struct winsize w = get_terminal_size();
    txt_handle->terminal.height = w.ws_row;
    txt_handle->terminal.width = w.ws_col;

    txt_handle->box.height = txt_handle->levels;

    // Use widest textbox as box width
    txt_handle->box.width = 0;
    for (int i = 0; i < txt_handle->levels; i++) {
        size_t len_i = strlen(txt_handle->text[i].text);
        if (len_i > txt_handle->box.width) {
            txt_handle->box.width = len_i;
        }
    }

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

    signal(SIGINT, sig_handler);

    hide_cursor();

    while (keep_running) {
        clear_terminal();

        render_text(txt_handle);

        usleep(SLEEP_PERIOD_US);
    }

    show_cursor();
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void sig_handler(int sig){
    keep_running = 0;
}

static struct winsize get_terminal_size(void) {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return w;
}

static int count_substrings(const char* string, const char* substring) {
    int count = 0;
    const char* tmp = string;
    while ((tmp = strstr(tmp, substring))) {
        count++;
        tmp++;
    }
    return count;
}

static void format_text(const char* text, struct text_level** formatted_text,
                        uint32_t* levels) {
    *levels = 0;
    *formatted_text = malloc(sizeof(struct text_level) *
                             (count_substrings(text, NEWLINE_DELIMITER) + 1));
    char* dummy = (char*)text;
    char* tok;
    while ((tok = strtok(dummy, NEWLINE_DELIMITER))) {
        (*formatted_text)[*levels].text_len = strlen(tok);
        (*formatted_text)[*levels].text =
            malloc(sizeof(char) * (*formatted_text)[*levels].text_len);
        memcpy((*formatted_text)[*levels].text, tok,
               (*formatted_text)[*levels].text_len);
        (*levels)++;
        tok += strlen(tok);
        dummy = NULL;
    }
}

static void clear_terminal(void) { printf("\e[1;1H\e[2J"); }

static void hide_cursor(void){
    printf("\e[?25l");
}

static void show_cursor(void){
    printf("\e[?25h");
}

static void render_text(struct text_handle* handle) {
    static int x_direction = STEP_SIZE_X, y_direction = STEP_SIZE_Y;
    handle->x += x_direction;
    handle->y += y_direction;

    for (int y = 0; y < handle->terminal.height; y++) {
        if (y == 0 || y + 1 == handle->terminal.height) {
            for (int k = 0; k < handle->terminal.width; k++) {
                printf("%c", WALL_CHAR);
            }
            continue;
        }
        printf("%c", LINEBREAK_CHAR);
        for (int x = 0; x < handle->terminal.width; x++) {
            for (int i = 0; i < handle->levels; i++) {
                if (y == handle->y + i && x == handle->x) {
                    printf("%s", handle->text[i].text);

                    x += handle->text[i].text_len;
                }
            }
            if (x == 0 || x + 1 == handle->terminal.width) {
                printf("%c", WALL_CHAR);
            } else {
                printf("%c", BACKGROUND_CHAR);
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
