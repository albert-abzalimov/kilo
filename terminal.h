
// terminal.h

#ifndef KILO_TERMINAL_H
#define KILO_TERMINAL_H

void enableRawMode(void);
void disableRawMode(void);
int editorReadKey(void);

int getWindowSize(int*, int*);
int getCursorPosition(int*, int*);

#endif
