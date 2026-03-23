#include "output.h"
#include "append_buffer.h"
#include "config.h"
#include <stdarg.h>
#include <stdint.h>
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
  editorDrawStatusBar(&ab);

  char buf[32];
  // the terminal is 1-indexed for cursor positions.. thats why we add + 1

  // move cursor!
  snprintf(buf, sizeof(buf), "\x1b[%d;%dH", (E.cursor_y - E.row_offset) + 1,
           (E.render_x - E.col_offset) + 1);

  abufAppend(&ab, buf, strlen(buf));

  // reveal the cursor
  abufAppend(&ab, "\x1b[?25h", 6);

  write(STDOUT_FILENO, ab.buf, ab.length);
  abufFree(&ab);
}

void editorDrawRows(struct abuf *ab) {
  for (int y = 0; y < E.screen_rows; y++) {
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
}

void editorDrawStatusBar(struct abuf *ab) {
  /*
The m command (Select Graphic Rendition) causes the text printed after it to be printed with various possible attributes including bold (1), underscore (4), blink (5), and inverted colors (7). For example, you could specify all of these attributes using the command <esc>[1;4;5;7m. An argument of 0 clears all attributes, and is the default argument, so we use <esc>[m to go back to normal text formatting.   */
  abufAppend(ab, "\x1b[7m", 4);

  char status[80], right_status[80];

  int len = snprintf(status, sizeof(status), "%.20s - %d lines", E.file_name, E.num_rows);

  int right_len = snprintf(right_status, sizeof(right_status), "%d:%d", E.cursor_y + 1, E.cursor_x);

  if (len > E.screen_cols) len = E.screen_cols;

  abufAppend(ab, status, len);

  while (len < E.screen_cols) {
    if (right_len == E.screen_cols - len) {
      abufAppend(ab, right_status, right_len);
      break;
    }
    abufAppend(ab, " ", 1);
    len++;
  }

  abufAppend(ab, "\x1b[m", 3);
}

// the ... is a variadic function
void editorSetStatusMessage(const char *fmt, ...) {
  // ap?
  // in order to retrieve these variable length arguements we use va_list;
  // so we call our argument list
  va_list argument_list;
  va_start(argument_list, fmt);
  // helps make our own printf style function
  vsnprintf(E.status_message, sizeof(E.status_message), fmt, argument_list);
  va_end(argument_list);
  // time(NULL) returns time
  E.status_message_time = time(NULL);

}


/// Runs every refresh frame
void editorScroll(void) {

  E.render_x = 0;
  // why is cursor_y < E.num_rows
  // OH BECAUSE CURSOR_Y IS ALLOWED TO BE at the end of the file
  if (E.cursor_y < E.num_rows) {
    E.render_x = editorRowCursorXToRenderX(&E.row[E.cursor_y], E.cursor_x);
  }
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

  if (E.render_x < E.col_offset) {
    E.col_offset = E.render_x;
  }
  if (E.render_x >= E.col_offset + E.screen_cols) {
    E.col_offset = E.render_x - E.screen_cols + 1;
  }
}

int32_t editorRowCursorXToRenderX(erow *row, int cursor_x) {
  // okay lets find
  int32_t render_x = 0;
  for (int i = 0; i < cursor_x; i++) {
    if (row->chars[i] == '\t') {
      // how many spaces are we to the right of the previous tab_stop?
      int32_t remainder = render_x % KILO_TAB_STOP;
      // how many spaces are we to the left of the next tab_stop?
      int32_t until_next_without_1 = KILO_TAB_STOP - 1 - remainder;
      render_x += until_next_without_1;
    }
    // since this always runs, we go one before the next tab_stop
    render_x++;
  }

  return render_x;
}
// _ _ _ t _
// 0 0 0 0 0 
