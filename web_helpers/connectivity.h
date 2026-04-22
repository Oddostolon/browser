#include <netdb.h>

int connect_to_host(const char* hostname);

int dns_request(const char* hostname, struct addrinfo **res);

int connect_socket(struct addrinfo *result);
