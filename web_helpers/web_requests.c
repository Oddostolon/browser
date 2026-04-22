#define _GNU_SOURCE
#include <stdlib.h>
#include <poll.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "web_requests.h"

int wr_get(int socket_fd, char* path, char* hostname)
{
	char request[1024] = {'\0'};
	
	snprintf(request, sizeof(request), 
			"GET %s HTTP/1.1\r\n"
			"Host: %s\r\n"
			"Connection: close\r\n"
			"\r\n",
			path, hostname);
	size_t request_len = strlen(request);

	ssize_t size_sent = 0;
	size_t total_sent = 0;
	while(total_sent < request_len)
	{
		if((size_sent = send(socket_fd, request + total_sent, request_len - total_sent, 0)) <= 0)
		{
			fprintf(stderr, "send failed\n");
			return -1;
		}

		total_sent += size_sent;
	}
	
	return 0;
}
