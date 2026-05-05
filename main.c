#include <bits/types/siginfo_t.h>
#include <openssl/crypto.h>
#include <string.h>
#include <sys/poll.h>
#include <fcntl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <err.h>
#include <signal.h>

#include "web_helpers/parse_url.h"
#include "web_helpers/connectivity.h"
#include "web_helpers/web_requests.h"
#include "tui/tui.h"
#include "web_helpers/ssl_helpers.h"
#include "actions/open_document.h"

volatile sig_atomic_t sigint_received = 0;

void sigint_handler(int s)
{
	sigint_received = 1;
}

int main(int argc, char const* argv[])
{
	tui_setup();
	noecho();

	struct sigaction sa;
	sa.sa_handler = sigint_handler;
	if(-1 == sigaction(SIGINT, &sa, NULL))
	{
		perror("sigaction failed");
		return -1;
	}

	char *document = open_document();
	if(NULL != document)
	{
		print_text(document, strlen(document));
	}

	keypad(stdscr, TRUE);

	int ch;
	while(!sigint_received)
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
				text_scroll(ch);
				break;

			case '/':
				free(document);
				document = open_document();
				if(NULL != document)
				{
					print_text(document, strlen(document));
				}
				break;
		}
	}

	free(document);
	tui_shutdown();
	return 0;
}
