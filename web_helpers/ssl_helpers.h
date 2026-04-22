#include <openssl/prov_ssl.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

static void openssl_init(void);
SSL_CTX *create_ssl_context(void);
