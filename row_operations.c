
#include "row_operations.h"
#include "config.h"
#include <stdlib.h>
#include <string.h>

// We want editorAppendRow() to allocate space for a new erow, and then copy the given string to a new erow at the end of the E.row array.

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
  E.num_rows++;
}
