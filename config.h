
// config.h

#ifndef KILO_CONFIG_H
#define KILO_CONFIG_H

#include <stdint.h>
#include <termios.h>


// defines a macro that bitwise ANDS with the value 0x1f, which is 00011111
// in ASCII, Ctrl keys simply zero out the first 3 bits
#define CTRL_KEY(k) ((k) & 0x1f)
#define KILO_TAB_STOP 4

#define KILO_VERSION "0.0.1"

enum editorKey {
	ARROW_LEFT = 1000,
	ARROW_RIGHT,
	ARROW_UP,
	ARROW_DOWN,
	DEL_KEY,
	PAGE_UP,
	PAGE_DOWN,
};

// ??/?
// Global storage for the original terminal attributes
// 'extern' keyword tells the compiler that this variable is defined elsewhere (in a .c file)
// row of text

typedef struct erow {
	int32_t size;
	int32_t render_size;
	char *chars;
	char *render;
} erow;

struct EditorConfig {
	int32_t cursor_x, cursor_y;
	int32_t render_x; // an index int32_to the row.render[]
	int32_t row_offset;
	int32_t col_offset;
	int32_t screen_rows;
	int32_t screen_cols;
	int32_t num_rows;
	erow *row;
	struct termios original_termios;
};

extern struct EditorConfig E;


// The 'die' function is utility, often grouped with configuration or main
void die(const char *s);

#endif
