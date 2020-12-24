#include "conn.h"

using namespace std;

ClientConnection::ClientConnection(const char *ca_cert, const char *my_cert, const char *my_key) 
	: Connection(ca_cert, my_cert, my_key) {
	ClientConnection::set_sock();
}

ClientConnection::~ClientConnection() {
	// BIO_free(sbio);
	SSL_shutdown(ssl);
	SSL_free(ssl);
}

void ClientConnection::set_sock() {
    Connection::set_sock();
    
    he = gethostbyname("localhost"); // gethostbyname(HOST_NAME);
	memcpy(&sin.sin_addr, (struct in_addr *)he->h_addr, he->h_length);
	if (connect(sock, (struct sockaddr *)&sin, sizeof sin) < 0) {
		perror("connect");
		exit(2);
	}
}

void ClientConnection::connect_server() {
	ssl = SSL_new(ctx);
	Connection::set_bio();

	string s = "";
    err = SSL_connect(ssl);

	if (SSL_connect(ssl) != 1) {
		switch (SSL_get_error(ssl, err)) {
			case SSL_ERROR_NONE: s="SSL_ERROR_NONE"; break;
			case SSL_ERROR_ZERO_RETURN: s="SSL_ERROR_ZERO_RETURN"; break;
			case SSL_ERROR_WANT_READ: s="SSL_ERROR_WANT_READ"; break;
			case SSL_ERROR_WANT_WRITE: s="SSL_ERROR_WANT_WRITE"; break;
			case SSL_ERROR_WANT_CONNECT: s="SSL_ERROR_WANT_CONNECT"; break;
			case SSL_ERROR_WANT_ACCEPT: s="SSL_ERROR_WANT_ACCEPT"; break;
			case SSL_ERROR_WANT_X509_LOOKUP: s="SSL_ERROR_WANT_X509_LOOKUP"; break;
			case SSL_ERROR_WANT_ASYNC: s="SSL_ERROR_WANT_ASYNC"; break;
			case SSL_ERROR_WANT_ASYNC_JOB: s="SSL_ERROR_WANT_ASYNC_JOB"; break;
			case SSL_ERROR_SYSCALL: s="SSL_ERROR_SYSCALL"; break;
			case SSL_ERROR_SSL: s="SSL_ERROR_SSL"; break;
		}
		cerr << "SSL error: " << s << endl;
		ERR_print_errors_fp(stderr);
		exit(3);
	}
}