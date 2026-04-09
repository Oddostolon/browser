#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "connectivity.h"


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

int connect_socket()
{
	int socket_fd = 0;
	struct addrinfo *result, *rp;

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

SSL_CTX *create_ssl_ctx(void)
{
	const SSL_METHOD *method = TLS_client_method();
	SSL_CTX *ctx = SSL_CTX_new(method);

	return ctx;
}
