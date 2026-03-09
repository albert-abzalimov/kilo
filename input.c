#include "terminal.h"
#include <stdlib.h>
#include "config.h"
#include <unistd.h>

#include "input.h"


void editorMoveCursor(int key) {
  // why null?
  // "since E.cursor_y is allowed to be one past the last line of the file"
  // we check to see if it is in an actual line
  erow *row = (E.cursor_y >= E.num_rows) ? NULL : &E.row[E.cursor_y];
  switch (key) {

    case 'j':
    case ARROW_DOWN:
      if (E.cursor_y < E.num_rows)
        E.cursor_y++;
      break;

    case 'k':
    case ARROW_UP:
      if (E.cursor_y != 0)
        E.cursor_y--;
      break;

    case 'h':
    case ARROW_LEFT:
      if (E.cursor_x != 0)
        E.cursor_x--;
      break;

    case 'l':
    case ARROW_RIGHT:
      // if row is valid, and our cursor is inside the row's size, then we can move
      if (row && E.cursor_x < row->size) {
        E.cursor_x++;
      }
      break;
  }

  // Clamp cursor if we moved out of bounds 
  // lets check if our cursor is in a valid position
  row = (E.cursor_y >= E.num_rows) ? NULL : &E.row[E.cursor_y];
  int row_len = row ? row->size : 0;
  if (E.cursor_x > row_len) {
    E.cursor_x = row_len;
  }
}

void editorProcessKeypress(void) {
  int c = editorReadKey();

  switch (c) {
    case CTRL_KEY('q'):

    	write(STDOUT_FILENO, "\x1b[2J", 4);
    	write(STDOUT_FILENO, "\x1b[H", 3);
      exit(0);
      break;


    case ARROW_UP:
    case ARROW_DOWN:
    case ARROW_RIGHT:
    case ARROW_LEFT:
    case 'j':
    case 'k':
    case 'l':
    case 'h':
      editorMoveCursor(c);
      break;

    case PAGE_DOWN:
    case PAGE_UP:
      // we need to create this code block because
      // You can’t declare variables directly inside a switch statement.
      {
        int times = E.screen_rows;
        while (times--) {
          editorMoveCursor(c == PAGE_UP ? ARROW_UP : ARROW_DOWN);
        }
        break;
      }

    case DEL_KEY:
      // we shall do this later!
      // TODO
      break;
  }
}
