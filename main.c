#include <ncurses.h>
#include <openssl/crypto.h>
#include <sys/poll.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <err.h>

#include "web_helpers/parse_url.h"
#include "web_helpers/connectivity.h"
#include "web_helpers/web_requests.h"
#include "tui/tui.h"
#include "web_helpers/ssl_helpers.h"

int main(int argc, char const* argv[])
{
	TUI tui;
	tui_setup(&tui);
	
	int socket_fd;
	char hostname[1024] = { '\0' };
	char path[1024] = { '\0' };
	char mode[6] = { '\0' };
	
	// Get first URL, load first website
	char *url = read_url(tui.url_window);

	if(-1 == parse_url(url, mode, hostname, path))
	{
		wclear(tui.text_window);
		wprintw(tui.text_window, "Failed to resolve URL.");
		wrefresh(tui.text_window);
	}
	else
	{
		socket_fd = connect_to_host(hostname);
		wr_get(socket_fd, path, hostname);
	}
	
	free(url);
	
	// website is in socket_fd
	

	// load website into text window
	char buffer[4096];
	ssize_t bytes_read = 0;
	ssize_t lines_printed = -1;
	size_t max_lines = 5000;
	while(1)
	{
		bytes_read = read(socket_fd, buffer, 4096);
		if(bytes_read == 0)
		{
			break;
		}
		if(bytes_read == -1)
		{
			wclear(tui.text_window);
			wprintw(tui.text_window, "%s\n", strerror(errno));
			prefresh(tui.text_window, 0, 0, 1, 1, LINES - 5, COLS - 2);
			break;
		}

		for (int i = 0; i < bytes_read; i++)
		{
			if(buffer[i] == '\r')
			{
				buffer[i] = ' ';
			}
			if(buffer[i] == '\n' || (i % COLS - 2) == 0)
			{
				lines_printed++;
				if(lines_printed == max_lines)
				{
					wresize(tui.text_window, max_lines + 5000, COLS - 2);
					max_lines += 5000;
				}
			}
			waddch(tui.text_window, buffer[i]);
		}
	}

	
	noecho();
	keypad(stdscr, TRUE);

	int ch;
	size_t pad_row = 0;
	while(1)
	{
		prefresh(tui.text_window, pad_row, 0, 1, 1, LINES - 5, COLS - 2);
		ch = getch();
		
		if(ch == 'q')
		{
			break;
		}

		switch(ch)
		{
			case KEY_UP:
				if(pad_row > 0)
				{
					pad_row--;
				}
				break;
			
			case KEY_DOWN:
				if(pad_row < max_lines)
				{
					pad_row++;
				}
				break;
		}
	}

	close(socket_fd);
	tui_shutdown();
	return 0;
}
