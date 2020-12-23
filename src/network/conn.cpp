#include "conn.h"

using namespace std;

Connection::Connection(const char *ca_cert, const char *my_cert, const char *my_key) {
    this->ca_cert = ca_cert;
    this->my_cert = my_cert;
    this->my_key = my_key;
    /* load encryption & hash algorithms for SSL */  
    SSL_library_init();    
    /* load the error strings for good error reporting */    	
	SSL_load_error_strings(); 

    meth = TLS_method(); 
	ctx = SSL_CTX_new(meth);
    Connection::set_certs();
    ssl = SSL_new(ctx);
}

Connection::~Connection() {
    SSL_shutdown(ssl);
    // second call is sent to peer
    SSL_shutdown(ssl); 
    SSL_CTX_free(ctx);
    SSL_free(ssl);
    BIO_free(sbio);

    close(sock);
    EVP_cleanup();
}

void Connection::set_certs() {
    /* Load client/server certificate into the SSL context */
    if (SSL_CTX_use_certificate_file(ctx, my_cert, SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr); 
        exit(1);
    }
 
    /* Load the client/server private key into the SSL context */
    if (SSL_CTX_use_PrivateKey_file(ctx, my_key, SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr); 
        exit(1);
    }   
 
    /* Load trusted CA */
    if (!SSL_CTX_load_verify_locations(ctx, ca_cert, NULL)) { 
        ERR_print_errors_fp(stderr); 
        exit(1);
    }

    /* Set to require peer (client) certificate verification */
    SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, NULL); // SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, verify_callback);
    /* Set the verification depth to 1 because client certificate has to be directly signed by CA */
    SSL_CTX_set_verify_depth(ctx, 1);
}

void Connection::set_sock() {
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock < 0) {
		perror("unable to create socket");
		exit(1);
	}

    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(DEFAULT_PORT);
}

void Connection::set_bio() {
    sbio = BIO_new(BIO_s_socket());
    BIO_set_fd(sbio, sock, BIO_NOCLOSE);
    SSL_set_bio(ssl, sbio, sbio);
}