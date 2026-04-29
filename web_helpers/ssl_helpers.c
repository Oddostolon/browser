#include <openssl/bio.h>
#include <openssl/prov_ssl.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/tls1.h>

static void openssl_init(void)
{
	SSL_load_error_strings();
	OpenSSL_add_ssl_algorithms();
}

SSL *create_ssl_object(char *hostname, int socket_fd)
{
	openssl_init();

	SSL_CTX *ctx = SSL_CTX_new(TLS_client_method());
	if(NULL == ctx)
	{
		return NULL;
	}

	SSL_CTX_set_mode(ctx, SSL_MODE_ENABLE_PARTIAL_WRITE);
	SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);
	if(0 == SSL_CTX_set_default_verify_paths(ctx))
	{
		return NULL;
	}
	if(0 == SSL_CTX_set_min_proto_version(ctx, TLS1_2_VERSION))
	{
		return NULL;
	}
	
	SSL *ssl = SSL_new(ctx);
	SSL_CTX_free(ctx);
	if(NULL == ssl)
	{
		return NULL;
	}

	if(0 == SSL_set_tlsext_host_name(ssl, hostname))
	{
		return NULL;
	}
	if(0 == SSL_set1_host(ssl, hostname))
	{
		return NULL;
	}
	SSL_set_fd(ssl, socket_fd);

	if(1 != SSL_connect(ssl))
	{
		return NULL;
	}
	if(X509_V_OK != SSL_get_verify_result(ssl))
	{
		return NULL;
	}

	return ssl;
}

