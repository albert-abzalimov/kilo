#include <string.h>
#define _DEFUALT_SOURCE

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <termios.h>

#include "config.h"
#include "file.h"
#include "terminal.h"
#include "input.h"
#include "output.h"


struct EditorConfig E;


void die(const char *s) {

  // Clear the screen
  // reposition cursor to the top left

	write(STDOUT_FILENO, "\x1b[2J", 4);
	write(STDOUT_FILENO, "\x1b[H", 3);
	
	perror(s);
	exit(1);
}

void initEditor(void) {
	E.cursor_x = 0;
	E.cursor_y = 0;
	E.render_x = 0;
	E.row_offset = 0;
	E.col_offset = 0;
	E.num_rows = 0;
	E.row = NULL;
	E.status_message[0] = '\0';
	E.status_message_time = 0;
	E.file_name = malloc(10 * sizeof(char));	
	if (E.file_name == NULL) {
		// uhoh!
		die("insufficent memory, file_name malloc");
	}
	strcpy(E.file_name, "[No Name]");

	// or i could actually write:
	// E.file_name = strdup("[No Name]");
	// if (E.file_name == NULL) die!!!
	
	if (getWindowSize(&E.screen_rows, &E.screen_cols) == -1) {
		die("get window size failed....");
	}

	// this is because we need the status bar and such
	E.screen_rows -= 2;
	
}


int main(int32_t argc, char *argv[]) {

	enableRawMode();
	initEditor();

	// if the user added arguments, open a file
	if (argc >= 2) {
		editorOpen(argv[1]);
	}

	editorSetStatusMessage("HELP: Ctrl-Q to quit");

	while (1) {
		editorRefreshScreen();
		editorProcessKeypress();
	}
	return 0;
}
