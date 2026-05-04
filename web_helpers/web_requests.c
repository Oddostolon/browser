#include <openssl/err.h>
#include <openssl/ssl.h>
#include <stdlib.h>
#include <poll.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "web_requests.h"

int wr_get(int socket_fd, SSL* ssl, char* hostname, char* path)
{
	char request[1024] = { '\0' };

	size_t request_len = 1024;
	snprintf(request, request_len, 
			"GET %s HTTP/1.1\r\n"
			"Host: %s\r\n"
			"Connection: close\r\n"
			"\r\n",
			path, hostname);
	
	if(NULL == ssl)
	{
		return write(socket_fd, request, request_len);
	}
	else
	{
		return SSL_write(ssl, request, strlen(request));
	}
}
