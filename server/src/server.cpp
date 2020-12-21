/**
 * <version> <status-code> <text>

	<option-lines>

	<newline>

	<body>
*/

#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#include <openssl/ssl.h>
#include <openssl/bio.h>
#include <openssl/err.h>

using namespace std;

// getcert
// verify username/password, generate cert from csr, and send and store cert 

// changepw
void send_http_resp(BIO *bio, const string &line, const string &host) {
    string resp = line + "\n\n";
}

// sendmsg

// recvmsg

int main(int argc, char **argv) {
    const char *CA_CERT = (*argv)++;
    const char *SERVER_CERT = (*argv)++;
    const char *SERVER_KEY = (*argv)++;

    SSL_CTX *ctx;
	SSL *ssl;
    const SSL_METHOD *meth;
	BIO *sbio;
	int err; char *s;

    int ilen;
	char ibuf[512];
	char *obuf = "GET / HTTP/1.0\n\n";

	struct sockaddr_in sin;
	int sock;
	struct hostent *he;

    SSL_library_init(); /* load encryption & hash algorithms for SSL */         	
	SSL_load_error_strings(); /* load the error strings for good error reporting */

    meth = TLS_server_method(); // TODO change to SSL?
	ctx = SSL_CTX_new(meth);
    ssl = SSL_new(ctx);

    /* Load server certificate into the SSL context */
    if (SSL_CTX_use_certificate_file(ctx, SERVER_CERT, SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors(bio_err);  /* == ERR_print_errors_fp(stderr); */
        exit(1);
    }
 
    /* Load the server private-key into the SSL context */
    if (SSL_CTX_use_PrivateKey_file(ctx, SERVER_KEY, SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors(bio_err);  /* == ERR_print_errors_fp(stderr); */
        exit(1);
    }   
 
    /* Load trusted CA */
    if (!SSL_CTX_load_verify_locations(ctx, CA_CERT, NULL)) { 
        ERR_print_errors(bio_err);  /* == ERR_print_errors_fp(stderr); */
        exit(1);
    }
 
    /* Set to require peer (client) certificate verification */
    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, verify_callback);
    /* Set the verification depth to 1 */
    SSL_CTX_set_verify_depth(ctx, 1);

    sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    CHK_ERR(sock, "socket");
    
    memset(&sa_serv, 0, sizeof(sa_serv));
    sa_serv.sin_family      = AF_INET;
    sa_serv.sin_addr.s_addr = INADDR_ANY;
    sa_serv.sin_port        = htons(s_port);      /* Server Port number */
    
    err = bind(sock, (struct sockaddr*)&sa_serv, sizeof(sa_serv));
    CHK_ERR(err, "bind");
    
    /* Receive a TCP connection. */
    err = listen(sock, 5);
    CHK_ERR(err, "listen");

    sock = accept(sock, (struct sockaddr*)&sa_cli, &client_len);
    BIO_printf(bio_c_out, "Connection from %lx, port %x\n", 
    sa_cli.sin_addr.s_addr, sa_cli.sin_port);
}