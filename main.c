#define _GNU_SOURCE
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
#include "parse_url.h"
#include "connectivity.h"
#include "web_requests.h"
#include <err.h>

#define BUFLEN 4096

int main(int argc, char const* argv[])
{
	char url[] = "http://browser.engineering/http.html";

	int socket_fd;
	char hostname[1024] = {'\0'};
	char path[1024] = {'\0'};
	char request[1024];
	struct addrinfo *result, *rp;

	parse_url(url, strlen(url), hostname, path);
	dns_request(hostname, &result);

	for (rp = result; rp != NULL; rp = rp->ai_next) 
	{
		socket_fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (socket_fd == -1)
		{
			continue;
		}
		
		if (connect(socket_fd, rp->ai_addr, rp->ai_addrlen) != -1)
		{
			break;
		}
		
		close(socket_fd);
	}

	freeaddrinfo(result);

	if (rp == NULL) 
	{               
		fprintf(stderr, "Could not connect\n");
		close(socket_fd);
	}
	


	int pipe_fd[2];
	if(pipe(pipe_fd) == -1)
	{
		fprintf(stderr, "Failed to create pipe (%s), exiting...\n", strerror(errno));
		return -1;
	}

	pid_t pid = fork();
	if(pid == -1)
	{
		fprintf(stderr, "Failed to fork (%s), exiting...\n", strerror(errno));
		close(pipe_fd[0]);
		close(pipe_fd[1]);
		close(socket_fd);
		return -1;
	}

	else if(pid == 0)
	{
		// Child case
		close(pipe_fd[0]);
		exit(wr_get(pipe_fd[1], socket_fd, path, hostname) == -1 ? EXIT_FAILURE : EXIT_SUCCESS);
		
	}
	else
	{
		// Parent case
		close(pipe_fd[1]);
		struct pollfd pfd = 
		{
			.fd = pipe_fd[0],
			.events = POLLIN
		};
		int readcounter = 0;
		while(1)
		{
			poll(&pfd, 1, -1);
			if(pfd.revents & POLLIN)
			{
				splice(pipe_fd[0], NULL, STDOUT_FILENO, NULL, 4096, 0);
			}
			else if(pfd.revents & POLLHUP)
			{
				break;
			}
		}
		close(pipe_fd[0]);
	}
	
	close(socket_fd);
}
