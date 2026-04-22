#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include "connectivity.h"
#include "parse_url.h"

int connect_to_host(const char* hostname)
{
	int socket_fd = -1;
	char path[1024] = { '\0' };
	struct addrinfo *result, *rp;

	dns_request(hostname, &result);
	socket_fd = connect_socket(result);
	
	return socket_fd;
}

int dns_request(const char* hostname, struct addrinfo **res)
{
	struct addrinfo hints; 
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = 0;

	int gai_rc = getaddrinfo(hostname, "80", &hints, res);

	if(gai_rc != 0)
	{
		fprintf(stderr, "Failed to resolve domain name '%s' (%s)", hostname, strerror(errno));
		return gai_rc;
	}
	
	return 0;
}

int connect_socket(struct addrinfo *result)
{
	int socket_fd = 0;
	struct addrinfo *rp;

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
		return -1;
	}

	return socket_fd;
}

