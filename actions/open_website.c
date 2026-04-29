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

#include "../web_helpers/parse_url.h"
#include "../tui/tui.h"
#include "../web_helpers/connectivity.h"
#include "../web_helpers/web_requests.h"
#include "../web_helpers/ssl_helpers.h"

int open_website(TUI *tui)
{
	echo();

	char hostname[1024] = { '\0' };
	char path[1024] = { '\0' };
	enum MODE mode;

	char *url = read_url(tui);
	int parse_result = parse_url(url, &mode, hostname, path);
	free(url);
	if(-1 == parse_result)
	{
		print_error("Failed to parse URL", tui);
		return -1;
	}

	int socket_fd = -1;
	SSL *ssl = NULL;
	switch(mode)
	{
		case http:
			socket_fd = connect_to_host(hostname, "80");
			if(-1 == socket_fd)
			{
				print_error("Failed to connect to the host", tui);
				return -1;
			}
			if(-1 == wr_get(socket_fd, path, hostname))
			{
				print_error("Failed to fetch website", tui);
				return -1;
			}
			break;

		case https:
			socket_fd = connect_to_host(hostname, "443");
			if(-1 == socket_fd)
			{
				print_error("Failed to connect to the host", tui);
				return -1;
			}
			ssl = create_ssl_object(hostname, socket_fd);
			if(NULL == ssl)
			{
				print_error("Failed to establish secure connection", tui);
				return -1;
			}
			if(-1 == wr_get_tls(socket_fd, ssl, path, hostname))
			{
				print_error("Failed to fetch website", tui);
				return -1;
			}
			break;
	}

	clear_text(tui);
	char buffer[4096];
	ssize_t bytes_read = 0;
	while(1)
	{
		switch(mode)
		{
			case http:
				bytes_read = read(socket_fd, buffer, 4096);
				break;

			case https:
				bytes_read = SSL_read(ssl, buffer, 4096);
				break;
		}
		if(0 == bytes_read)
		{
			break;
		}
		if(-1 == bytes_read)
		{
			print_error(strerror(errno), tui);
			break;
		}

		print_text(buffer, bytes_read, tui);
	}

	noecho();

	return 0;
}
