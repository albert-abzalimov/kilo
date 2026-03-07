#ifndef KILO_APPEND_BUFFER_H
#define KILO_APPEND_BUFFER_H

struct abuf {
  char *buf;
  int length;
};

#define ABUF_INIT {NULL, 0}

void abufAppend(struct abuf *, const char*, int);

void abufFree(struct abuf *);

#endif
