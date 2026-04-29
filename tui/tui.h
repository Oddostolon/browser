#pragma once

#include <ncursesw/ncurses.h>
#include <stdbool.h>

#define PAGE_SIZE 5000

typedef struct
{
	size_t current_line;
	size_t x, y, h, w;
	size_t lines_printed;
	size_t max_lines;
	WINDOW *pad;
} TEXT_WINDOW;

typedef struct 
{
	WINDOW* text_frame;
	TEXT_WINDOW text_window;
	WINDOW* url_frame;
	WINDOW* url_window;
	WINDOW* tooltip_window;
} TUI;

void tui_setup(TUI *tui);
void tui_shutdown(TUI *tui);

char * read_url(TUI *tui);

void print_text(char* text, size_t len, TUI *tui);
void print_error(char* text, TUI *tui);
void clear_text(TUI *tui);

void text_scroll(int ch, TUI *tui);

