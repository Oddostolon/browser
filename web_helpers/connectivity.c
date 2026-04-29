#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include "connectivity.h"
#include "parse_url.h"

int connect_to_host(const char* hostname, const char* port)
{
	struct addrinfo *res;
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	int gai_rc = getaddrinfo(hostname, port, &hints, &res);

	struct addrinfo *rp;

	int socket_fd = -1;
	for(rp = res; rp != NULL; rp = rp->ai_next)
	{
		socket_fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if(socket_fd == -1)
		{
			continue;
		}
		
		if(-1 != connect(socket_fd, rp->ai_addr, rp->ai_addrlen))
		{
			break;
		}

		close(socket_fd);
	}

	freeaddrinfo(res);

	return socket_fd;
}

