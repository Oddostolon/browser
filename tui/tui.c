#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include "tui.h"



void tui_setup(TUI *tui)
{
	initscr();
	cbreak();
	refresh();

	tui->text_frame = newwin(LINES - 3, COLS, 0, 0);
	box(tui->text_frame, 0, 0);
	wrefresh(tui->text_frame);
	tui->text_window = newpad(5000, COLS - 2);

	tui->url_frame = newwin(3, COLS, LINES - 4, 0);
	wborder(tui->url_frame, 0, 0, 0, 0, ACS_LTEE, ACS_RTEE, 0, 0);
	wrefresh(tui->url_frame);
	tui->url_window = subwin(tui->url_frame, 1, COLS - 2, LINES - 3, 1);

	tui->tooltip_window = newwin(1, COLS, LINES - 1, 0);
}

char * read_url(WINDOW *win)
{
	char buffer[1024] = { '\0' };
	if(ERR == wgetnstr(win, buffer, 1023))
	{
		return NULL;
	}
	
	char *retval = malloc(strlen(buffer) + 1);
	if (!retval)
	{
		return NULL;
	}

	strcpy(retval, buffer);
	return retval;
}

void print_text(char* buffer, WINDOW* win)
{
	wprintw(win, "%s", buffer); 	
	wrefresh(win);
}

void tui_shutdown()
{
	endwin();
}
