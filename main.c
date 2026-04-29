#include <openssl/crypto.h>
#include <sys/poll.h>
#include <fcntl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <err.h>

#include "web_helpers/parse_url.h"
#include "web_helpers/connectivity.h"
#include "web_helpers/web_requests.h"
#include "tui/tui.h"
#include "web_helpers/ssl_helpers.h"
#include "actions/open_website.h"

int main(int argc, char const* argv[])
{
	TUI tui;
	tui_setup(&tui);
	noecho();

	int socket_fd = open_website(&tui);

	keypad(stdscr, TRUE);

	int ch;
	while(1)
	{
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
				socket_fd = open_website(&tui);
				break;
		}
	}

	close(socket_fd);
	tui_shutdown(&tui);
	return 0;
}
