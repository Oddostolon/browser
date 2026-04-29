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

size_t build_get_request(char* request, int len, char* path, char* hostname)
{
	snprintf(request, len, 
			"GET %s HTTP/1.1\r\n"
			"Host: %s\r\n"
			"Connection: close\r\n"
			"\r\n",
			path, hostname);

	return strlen(request);
}

int wr_get(int socket_fd, char* path, char* hostname)
{
	char request[1024] = {'\0'};
	
	size_t request_len = build_get_request(request, sizeof(request), path, hostname);

	ssize_t size_sent = 0;
	size_t total_sent = 0;
	while(total_sent < request_len)
	{
		if(0 >= (size_sent = send(socket_fd, request + total_sent, request_len - total_sent, 0)))
		{
			fprintf(stderr, "send failed\n");
			return -1;
		}

		total_sent += size_sent;
	}
	
	return 0;
}

int wr_get_tls(int socket_fd, SSL* ssl, char* path, char* hostname)
{
	char request[1024] = { '\0' };

	size_t request_len = 1024;
	snprintf(request, request_len, 
			"GET %s HTTP/1.1\r\n"
			"Host: %s\r\n"
			"Connection: close\r\n"
			"\r\n",
			path, hostname);

	return SSL_write(ssl, request, strlen(request));
}
