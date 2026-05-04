#include <openssl/ssl.h>

int wr_get(int socket_fd, SSL *ssl, char* hostname, char* path);
