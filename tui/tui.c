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

	tui->text_window = (TEXT_WINDOW)
	{
		.current_line = 0,
		.x = 1,
		.y = 1,
		.h = LINES - 5,
		.w = COLS - 2,
		.lines_printed = 0,
		.max_lines = PAGE_SIZE,
		.pad = newpad(PAGE_SIZE, COLS - 2)
	};

	tui->url_frame = newwin(3, COLS, LINES - 4, 0);
	wborder(tui->url_frame, 0, 0, 0, 0, ACS_LTEE, ACS_RTEE, 0, 0);
	wrefresh(tui->url_frame);
	tui->url_window = subwin(tui->url_frame, 1, COLS - 2, LINES - 3, 1);

	tui->tooltip_window = newwin(1, COLS, LINES - 1, 0);
}

void text_refresh(TUI *tui)
{
	prefresh(tui->text_window.pad, 
			tui->text_window.current_line, 0, 
			tui->text_window.y, tui->text_window.x, 
			tui->text_window.h, tui->text_window.w);
}

char * read_url(TUI *tui)
{
	char buffer[1024] = { '\0' };
	if(ERR == wgetnstr(tui->url_window, buffer, 1023))
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

void print_text(char* text, size_t len, TUI *tui)
{
	for(int i = 0; i < len; i++)
	{
		if(text[i] == '\r')
		{
			text[i] = ' ';
		}
		if(text[i] == '\n' || (i % tui->text_window.w) == 0)
		{
			tui->text_window.lines_printed += (i != 0);
			if(tui->text_window.lines_printed == tui->text_window.max_lines)
			{
				wresize(tui->text_window.pad, 
						tui->text_window.max_lines + PAGE_SIZE, 
						tui->text_window.w);
				tui->text_window.max_lines += PAGE_SIZE;
			}
		}

		waddch(tui->text_window.pad, text[i]);
	}
	text_refresh(tui);
}

void print_error(char* text, TUI *tui)
{
	wclear(tui->text_window.pad);
	wprintw(tui->text_window.pad, "%s", text);

	tui->text_window.current_line = 0;
	text_refresh(tui);
}

void clear_text(TUI *tui)
{
	wclear(tui->text_window.pad);

	text_refresh(tui);
}

void text_scroll(int ch, TUI *tui)
{
	size_t *line = &tui->text_window.current_line;
	switch(ch)
	{
		case KEY_UP:
			if(*line > 0)
			{
				*line -= 1;
			}
			break;

		case KEY_PPAGE:
			if(*line > tui->text_window.h)
			{
				*line -= tui->text_window.h;
			}
			else
			{
				*line = 0;
			}
			break;

		case KEY_DOWN:
			if(*line < tui->text_window.max_lines)
			{
				*line += 1;
			}
			break;

		case KEY_NPAGE:
			if(*line <= (tui->text_window.max_lines - tui->text_window.h))
			{
				*line += tui->text_window.h;
			}
			else
			{
				*line = tui->text_window.max_lines;
			}
			break;
	}

	text_refresh(tui);
}

void tui_shutdown(TUI *tui)
{
	delwin(tui->tooltip_window);
	delwin(tui->url_window);
	delwin(tui->url_frame);
	delwin(tui->text_window.pad);
	delwin(tui->text_frame);

	endwin();
}
