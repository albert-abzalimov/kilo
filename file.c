// lets do some file shit io here
//
#include "file.h"
#include "config.h"
#include "row_operations.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// eventually will be reading a file
void editorOpen(char *filename) {

  free(E.file_name);
  E.file_name = strdup(filename);

  // opens a file
  FILE *fp = fopen(filename, "r");
  // catch error
  if (!fp)
    die("fopen() at editorOpen");

  char *line = NULL;
  size_t line_cap = 0;
  ssize_t line_len;

  // retrieves the next line, and stores the lines length in the line_cap variable
  // the while loop works because it returns -1 when we reach EOF
  while ((line_len = getline(&line, &line_cap, fp)) != -1) {
    // while we still have some chars and the end is still a carriage return...
    // OH this is trimming!?
    // strips off the carriage return or newline, because why store it in
    // a E.row if we know its one line lmao!
    while (line_len > 0 &&
           (line[line_len - 1] == '\n' || line[line_len - 1] == '\r')) {
      line_len--;
    }

    editorAppendRow(line, line_len);
  }
  free(line);
  fclose(fp);
}
