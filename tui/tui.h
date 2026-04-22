#include <ncurses.h>

typedef struct 
{
	WINDOW *text_frame;
	WINDOW *text_window;
	WINDOW *url_frame;
	WINDOW *url_window;
	WINDOW *tooltip_window;
} TUI;

void tui_setup(TUI *tui);
char * read_url(WINDOW *win);
void print_text(char* buffer, WINDOW* win);
void tui_shutdown();

