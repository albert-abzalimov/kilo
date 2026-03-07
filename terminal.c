#include "terminal.h" // For function declarations
#include "config.h"
#include <asm-generic/ioctls.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

// takes pointers to rows and cols so you can actually modify them.. i guess...
int getWindowSize(int *rows, int *cols) {

  struct winsize windowSize;

  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &windowSize) == 1 ||
      windowSize.ws_col == 0) {

    if (write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12) {
      return -1;
    }

    return getCursorPosition(rows, cols);
  } else {
    *cols = windowSize.ws_col;
    *rows = windowSize.ws_row;
    // success
    return 0;
  }

  return 0;
}

int getCursorPosition(int *rows, int *cols) {

  char buffer[32];
  unsigned int i = 0;

  // send out the cursor pos request
  if (write(STDOUT_FILENO, "\x1b[6n", 4) != 4)
    return -1;

  while (i < sizeof(buffer) - 1) {
    // if reading gives an error...
    if (read(STDOUT_FILENO, &buffer[i], 1) != 1)
      break;

    if (buffer[i] == 'R')
      break;
    i++;
  }

  buffer[i] = '\0';

  for (unsigned int j = 0; j < i; j++) {
    printf("\r\n&buffer[%d]: '%s'\r\n", j, &buffer[j]);
  }

  if (buffer[1] != '[') {
    printf("obviously buffer[1] != '['");
  }
  if (buffer[0] != '\x1b') {
    printf("buffer[0] is not \\x1b");
  }

  // first lets make sure that we actually got an escape sequence
  if (buffer[0] != '\x1b' || buffer[1] != '[')
    return -1;

  // we are passing '24;80' kinda thing (buffer[2])
  // parses the buffer[2] based on the string literal "%d, %d"
  //  into the variables rows, cols

  int result = sscanf(&buffer[2], "%d, %d", rows, cols);

  if (result != 2) {
    printf("scannf failed reading the two integers");
    return -1;
  }

  return 0;
}

/// wait for one key press and return it.
// later we expand this to read multiple
// bytes to handle escape sequences.
int editorReadKey(void) {
  int ourRead;

  char c;

  while ((ourRead = read(STDIN_FILENO, &c, 1)) != 1) {
    if (ourRead == -1 && errno != EAGAIN) {
      die("read did not work in editorReadKey");
    }
  }

  if (c == '\x1b') {
    char seq[3];

    // if nothing else was read... then escape was just pressed and we can
    // return that
    if (read(STDIN_FILENO, &seq[0], 1) != 1)
      return '\x1b';
    if (read(STDIN_FILENO, &seq[1], 1) != 1)
      return '\x1b';

    if (seq[0] == '[') {

      if (seq[1] >= '0' && seq[1] <= '9') {
        if (read(STDOUT_FILENO, &seq[2], 1) != 1)
          return '\x1b';
        if (seq[2] == '~') {
          switch (seq[1]) {
            case '3':
              return DEL_KEY;
            case '5':
              return PAGE_UP;
            case '6':
              return PAGE_DOWN;
          }
        }
      } else {
        switch (seq[1]) {
        case 'A':
          return ARROW_UP;
        case 'B':
          return ARROW_DOWN;
        case 'C':
          return ARROW_RIGHT;
        case 'D':
          return ARROW_LEFT;
        }
      }
    }

    return '\x1b';
  }
  return c;
}

void enableRawMode(void) {
  // retrieve attributes
  if (tcgetattr(STDIN_FILENO, &E.original_termios) == -1) {
    die("tcsetattr failed in the beginning of enableRawMode");
  }

  // comes from stdlib.h, simply says what function to run before exit() or
  // main() returns
  atexit(disableRawMode);

  struct termios raw = E.original_termios;

  // bitwise AND operator
  // ECHO is a bit flag 000000000000000000000001000
  // NOT on this is     111111111111111111111110111
  // so the c_lflag "local flag" when AND with this only that specific 0 bit
  // changes how nifty ECHO: print out what you type ICANON: canonical mode
  // where you need to press "enter" before input is sent ISIG: turns off Ctrl-Z
  // and Ctrl-C IEXTEN: turns off Ctrl-V
  raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);

  // IXON: Ctrl-S and Ctrl-Q software control flow
  // ICRNL: fixes Ctrl-M
  raw.c_iflag &= ~(IXON | ICRNL);

  // turns off all output processing
  raw.c_oflag &= ~(OPOST);

  // VMIN value sets the minimum number of bytes of input needed before read()
  // can return setting to 0 means that as soon as we read() gets any input, it
  // returns it
  raw.c_cc[VMIN] = 0;
  // VTIME value is max time to wait before read returns. set in 0.1 of seconds
  raw.c_cc[VTIME] = 1;

  // EXTRA TRADITION STUFF??
  raw.c_cflag |= (CS8);
  raw.c_iflag &= ~(BRKINT | INPCK | ISTRIP);

  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) {
    die("tcsetattr failed at the end of enableRawMode");
  }
}

void disableRawMode(void) {

  // reset the attributes back to what it was in the beginning
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.original_termios) ==
      -1) {
    die("tcsetattr failed in disableRawMode");
  }

  printf("Exited kilo.\n");
}
