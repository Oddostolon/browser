#include <openssl/ssl.h>

int wr_get(int socket_fd, char* path, char* hostname);
int wr_get_tls(int socket_fd, SSL* ssl, char* path, char* hostname);
