#include "output.h"
#include "append_buffer.h"
#include "config.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void editorRefreshScreen(void) {
  // 4 bytes out to the termianl
  // first byte is '\x1b' which is the escape character
  // this is an "escape sequence"

  struct abuf ab = ABUF_INIT;

  // hide the cursor
  abufAppend(&ab, "\x1b[?25l", 6);

  // Clear the screen
  // abufAppend(&ab, "\x1b[2J", 4);

  // reposition cursor to the top left
  abufAppend(&ab, "\x1b[H", 3);

  editorScroll();
  editorDrawRows(&ab);

  char buf[32];
  // the terminal is 1-indexed for cursor positions.. thats why we add + 1

  // move cursor!
  snprintf(buf, sizeof(buf), "\x1b[%d;%dH", (E.cursor_y - E.row_offset) + 1,
           (E.cursor_x - E.col_offset) + 1);

  abufAppend(&ab, buf, strlen(buf));

  // reveal the cursor
  abufAppend(&ab, "\x1b[?25h", 6);

  write(STDOUT_FILENO, ab.buf, ab.length);
  abufFree(&ab);
}

void editorDrawRows(struct abuf *ab) {

  for (int y = 0; y < E.screen_rows - 1; y++) {
    int file_row = y + E.row_offset;
    // if visible in our screen? i am assuming?
    if (file_row >= E.num_rows) {
      if (E.num_rows == 0 && y == E.screen_rows / 3) {
        char welcome[80];
        int welcome_length =
            snprintf(welcome, sizeof(welcome), "Kilo Editor -- version %s",
                     KILO_VERSION);

        if (welcome_length > E.screen_cols)
          welcome_length = E.screen_cols;

        int padding = (E.screen_cols - welcome_length) / 2;

        if (padding) {
          abufAppend(ab, "~", 1);
          padding--;
        }

        while (padding--)
          abufAppend(ab, " ", 1);

        abufAppend(ab, welcome, welcome_length);
      } else {
        abufAppend(ab, "~", 1);
      }
    } else {
      int len = E.row[file_row].render_size - E.col_offset;
      if (len < 0) len = 0;
      // truncate if its too long
      if (len > E.screen_cols) {
        len = E.screen_cols;
      }
      abufAppend(ab, &E.row[file_row].render[E.col_offset], len);
    }
    // i forgot what these do. fuck.
    abufAppend(ab, "\x1b[K", 3);
    abufAppend(ab, "\r\n", 2);
  }
  abufAppend(ab, "~", 1);
  abufAppend(ab, "\x1b[K", 3);
}


void editorScroll(void) {
  // scroll up
  // this is the scenario where our cursor is above our visible window
  if (E.cursor_y < E.row_offset) {
    E.row_offset = E.cursor_y;
  }  

  // scroll down
  // this shit is starting to make sense (cursor is below)
  if (E.cursor_y >= E.row_offset + E.screen_rows) {
    E.row_offset = E.cursor_y - E.screen_rows + 1;
  }

  if (E.cursor_x < E.col_offset) {
    E.col_offset = E.cursor_x;
  }
  if (E.cursor_x >= E.col_offset + E.screen_cols) {
    E.col_offset = E.cursor_x - E.screen_cols + 1;
  }
}
