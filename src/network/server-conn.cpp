#include "conn.h"

using namespace std;

ServerConnection::ServerConnection(const char *ca_cert, const char *my_cert, const char *my_key) 
    : Connection(ca_cert, my_cert, my_key) {
    ServerConnection::set_sock();
    Connection::set_bio();
    ServerConnection::accept_client();
}

void ServerConnection::set_sock() {
    Connection::set_sock();
    sin.sin_addr.s_addr = htonl(INADDR_ANY);  

    if (bind(sock, (struct sockaddr*)&sin, sizeof(sin)) < 0) {
        perror("unable to bind");
        exit(2);
    }

    if (listen(sock, 5) < 0 ) {
        perror("unable to listen");
        exit(2);
    }
}

void ServerConnection::accept_client() {
    struct sockaddr_in sa_cli;
    uint len_cli = sizeof(sa_cli);
    int client = accept(sock, (struct sockaddr *)&sa_cli, &len_cli);
    if (client < 0) {
        perror("unable to accept");
        exit(1);
    }

    SSL_set_fd(ssl, client);
    if (SSL_accept(ssl) != 1) {
        ERR_print_errors_fp(stderr);
        exit(1);
    } else {
        int ilen;
        while ((ilen = SSL_read(ssl, ibuf, sizeof ibuf - 1)) > 0) {
            ibuf[ilen] = '\0';
            printf("%s", ibuf);
        }
    }

    // BIO_printf(bio_c_out, "Connection from %lx, port %x\n", sa_cli.sin_addr.s_addr, sa_cli.sin_port);
}