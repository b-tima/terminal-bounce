#ifndef EXAMPLE_LIB_H_
#define EXAMPLE_LIB_H_

#ifdef __cplusplus
extern "C" {
#endif


/*********************
 *      INCLUDES
 *********************/

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef void* terminal_handle_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void terminal_bounce_init(terminal_handle_t* handle, const char* text);

void terminal_bounce_play(terminal_handle_t* handle);

/**********************
 *  GLOBAL VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   POST INCLUDES
 *********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* EXAMPLE_LIB_H_ */