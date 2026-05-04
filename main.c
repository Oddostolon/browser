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

#include "web_helpers/parse_url.h"
#include "web_helpers/connectivity.h"
#include "web_helpers/web_requests.h"
#include "tui/tui.h"
#include "web_helpers/ssl_helpers.h"
#include "actions/open_document.h"

int main(int argc, char const* argv[])
{
	tui_setup();
	noecho();

	char *document = open_document();
	if(NULL != document)
	{
		print_text(document, strlen(document));
	}

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
