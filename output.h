

#ifndef KILO_OUTPUT_H
#define KILO_OUTPUT_H

#include "append_buffer.h"
#include "config.h"

void editorRefreshScreen(void);

void editorDrawRows(struct abuf*);

// this function will add to the row_offset global variable
// when our cursor moves out of the visible frame
void editorScroll(void);

int editorRowCursorXToRenderX(erow*, int);

void editorDrawStatusBar(struct abuf *);

void editorSetStatusMessage(const char* fmt, ...);

extern struct abuf ab;

#endif
