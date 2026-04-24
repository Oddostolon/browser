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

int visit_website(TUI *tui)
{
	int socket_fd = -1;
	char hostname[1024] = { '\0' };
	char path[1024] = { '\0' };
	char mode[6] = { '\0' };
	
	// Get first URL, load first website
	char *url = read_url(tui);

	if(-1 == parse_url(url, mode, hostname, path))
	{
		print_error("Failed to resolve URL...", tui);
	}
	else
	{
		socket_fd = connect_to_host(hostname);
		wr_get(socket_fd, path, hostname);
	}
	
	free(url);

	if(socket_fd == -1)
	{
		print_error("Failed to resolve URL...", tui);
		return -1;
	}

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
			print_error(strerror(errno), tui);
			break;
		}
		
		print_text(buffer, bytes_read, tui);
	}

	return socket_fd;
}

int main(int argc, char const* argv[])
{
	TUI tui;
	tui_setup(&tui);
	
	int socket_fd = visit_website(&tui);

	
	noecho();
	keypad(stdscr, TRUE);

	int ch;
	size_t pad_row = 0;
	while(1)
	{
		prefresh(tui.text_window.pad, pad_row, 0, 1, 1, LINES - 5, COLS - 2);
		ch = getch();
		
		if(ch == 'q')
		{
			break;
		}

		switch(ch)
		{
			case KEY_DOWN:
			case KEY_UP:
			case KEY_NPAGE:
			case KEY_PPAGE:
				text_scroll(ch, &tui);
				break;

			case '/':
				echo();
				visit_website(&tui);
				noecho();
				break;
		}
	}

	close(socket_fd);
	tui_shutdown(&tui);
	return 0;
}
