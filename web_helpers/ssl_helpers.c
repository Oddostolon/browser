#include <openssl/bio.h>
#include <openssl/prov_ssl.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/tls1.h>
#include <stdio.h>

static void openssl_init(void)
{
	SSL_load_error_strings();
	OpenSSL_add_ssl_algorithms();
}

SSL *create_ssl_object(char *hostname)
{
	SSL_CTX *ctx = SSL_CTX_new(TLS_client_method());
	if(!ctx)
	{
		ERR_print_errors_fp(stderr);
		return NULL;
	}

	SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);
	if(!SSL_CTX_set_default_verify_paths(ctx))
	{
		ERR_print_errors_fp(stderr);
		return NULL;
	}

	if(!SSL_CTX_set_min_proto_version(ctx, TLS1_2_VERSION))
	{
		ERR_print_errors_fp(stderr);
		return NULL;
	}
	
	SSL *ssl = SSL_new(ctx);
	if(ssl == NULL)
	{
		ERR_print_errors_fp(stderr);
		return NULL;
	}

	if(!SSL_set_tlsext_host_name(ssl, hostname))
	{
		ERR_print_errors_fp(stderr);
		return NULL;
	}

	if(!SSL_set1_host(ssl, hostname))
	{
		ERR_print_errors_fp(stderr);
		return NULL;
	}

	return ssl;
}

BIO *create_bio_object(int socket_fd)
{
	BIO *bio = BIO_new(BIO_s_socket());
	if(bio == NULL)
	{
		return NULL;
	}

	BIO_set_fd(bio, socket_fd, BIO_CLOSE);

	return bio;
}

int ssl_handshake(SSL *ssl)
{
	if(SSL_connect(ssl) < 1)
	{
		ERR_print_errors_fp(stderr);
		return -1;
	}

	if(SSL_get_verify_result(ssl) != X509_V_OK)
	{
		ERR_print_errors_fp(stderr);
		return -1;
	}

	return 0;
}
