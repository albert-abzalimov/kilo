#include "append_buffer.h"
#include <string.h>
#include <stdlib.h>


void abufAppend(struct abuf *ab, const char *str, int length) {

  // reallocate enough memory into our buffer with the new length
  char *new = realloc(ab->buf, ab->length + length);

  // if reallocating didnt work?
  if (new == NULL) return;

  // copy over the new string
  mempcpy(&new[ab->length], str, length);

  ab->buf = new;
  ab->length += length;
}

void abufFree(struct abuf *ab) {
  free(ab->buf);
}
