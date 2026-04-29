#include <openssl/prov_ssl.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

static void openssl_init(void);
SSL *create_ssl_object(char *hostname, int socket_fd);
BIO *create_bio_object(int socket_fd);
int ssl_handshake(SSL *ssl);
