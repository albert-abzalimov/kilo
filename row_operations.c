
#include "row_operations.h"
#include "config.h"
#include <stdlib.h>
#include <string.h>

void editorUpdateRow(erow *row) {

  int tabs = 0;

  for (int i = 0; i < row->size; i++) {
    if (row->chars[i] == '\t')
      tabs++;
  }

  free(row->render);
  row->render = malloc(row->size + tabs * (KILO_TAB_STOP - 1) + 1);

  int idx = 0;
  for (int i = 0; i < row->size; i++) {
    if (row->chars[i] == '\t') {
      row->render[idx++] = ' ';
      // go until we reach a tab stop (a column divisible by 8)
      while (idx % KILO_TAB_STOP != 0)
        row->render[idx++] = ' ';
    } else {
      row->render[idx++] = row->chars[i];
    }
  }
  row->render[idx] = '\0';
  row->render_size = idx;
}

// We want editorAppendRow() to allocate space for a new erow, and then copy the
// given string to a new erow at the end of the E.row array.
void editorAppendRow(char *s, size_t len) {

  // reallocate 1 more row
  E.row = realloc(E.row, sizeof(erow) * (E.num_rows + 1));

  int32_t at = E.num_rows;

  E.row[at].size = len;
  E.row[at].chars = malloc(len + 1);

  // copy the data from s into our *chars
  memcpy(E.row[at].chars, s, len);
  // manually add the null terminator
  E.row[at].chars[len] = '\0';

  // initialize
  E.row[at].render_size = 0;
  E.row[at].render = NULL;

  editorUpdateRow(&E.row[at]);

  E.num_rows++;
}
