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

char * open_website(int socket_fd, SSL* ssl, char *hostname, char *path)
{
	char *document = NULL;
	if(-1 == wr_get(socket_fd, ssl, hostname, path))
	{
		print_error("Failed to retrieve website at %s from %s", path, hostname);
		return NULL;
	}

	size_t docsize = 4096;
	document = malloc(docsize);
	
	char buffer[4096];
	ssize_t bytes_read = 0;
	size_t bytes_total = 0;
	while(1)
	{
		if(NULL != ssl)
		{
			bytes_read = SSL_read(ssl, buffer, 4096);
		}
		else
		{
			bytes_read = read(socket_fd, buffer, 4096);
		}
		if(0 == bytes_read)
		{
			break;
		}
		if(-1 == bytes_read)
		{
			print_error(strerror(errno));
			break;
		}

		if((bytes_total + bytes_read) > docsize)
		{
			docsize *= 2;
			document = realloc(document, docsize+1);
		}

		memcpy(document + bytes_total, buffer, bytes_read);
		bytes_total += bytes_read;
	}
	document[bytes_total] = '\0';

	return document;
}


char * open_file(char *path)
{
	FILE *fp = fopen(path, "r");
	if(fp == NULL)
	{
		print_error("Failed to open file");
		return NULL;
	}

	long size;
	if(fseek(fp, 0, SEEK_END) < 0 || (size = ftell(fp)) < 0)
	{
		print_error("Failed to determine file size");
		fclose(fp);
		return NULL;
	}

	char *document = malloc(size + 1);
	if(document == NULL)
	{
		print_error("Failed to allcoate memory for file content");
		fclose(fp);
		return NULL;
	}

	rewind(fp);
	size_t n = fread(document, 1, size, fp);
	if(n != size)
	{
		print_error("Mismatch between file size and bytes read. File size: %d. Bytes read: %d", size, n);
		return NULL;
	}
	document[size] = '\0';
	
	return document;
}

char * open_document()
{
	echo();

	char hostname[1024] = { '\0' };
	char path[1024] = { '\0' };
	enum MODE mode;

	char *url = read_url();
	int parse_result = parse_url(url, &mode, hostname, path);
	free(url);
	if(-1 == parse_result)
	{
		print_error("Failed to parse URL");
		return NULL;
	}

	if(file == mode)
	{
		return open_file(path);
	}

	int socket_fd = -1;
	SSL *ssl = NULL;
	
	char* port = (https == mode) ? "443" : "80";
	socket_fd = connect_to_host(hostname, port);
	if(-1 == socket_fd)
	{
		print_error("Failed to connect to host (%s)", hostname);
		return NULL;
	}
	if(https == mode)
	{
		ssl = create_ssl_object(hostname, socket_fd);
		if(NULL == ssl)
		{
			print_error("Failed to establish secure connection");
			return NULL;
		}
	}

	return open_website(socket_fd, ssl, hostname, path);
}
