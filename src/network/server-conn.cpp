#include <sstream>
#include <vector>
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

int ServerConnection::send_string(string to_send) {
    return SSL_write(ssl, to_send.c_str(), to_send.length() + 1);
}

int ServerConnection::accept_client() {
    struct sockaddr_in sa_cli;
    uint len_cli = sizeof(sa_cli);
    this->client = accept(sock, (struct sockaddr *)&sa_cli, &len_cli);
    if (client < 0) {
        perror("unable to accept");
        exit(1);
    }

    SSL_set_fd(ssl, client);
    if (SSL_accept(ssl) != 1) {
        ERR_print_errors_fp(stderr);
        exit(1);
    } 

    return -1;
    // BIO_printf(bio_c_out, "Connection from %lx, port %x\n", sa_cli.sin_addr.s_addr, sa_cli.sin_port);
}

REQ ServerConnection::parse_req(string req)
{
    vector<int> lines;
    int ind = -1;
    REQ to_ret;
    while((ind = req.find('\n', ind + 1)) != string::npos)
    {
        lines.push_back(ind);
    }
    if (lines.size() > 3)
    {
        to_ret.user = req.substr(lines[1] + 1, lines[2] - lines[1] - 1);
        to_ret.password = req.substr(lines[2] + 1, lines[3] - lines[2] - 1);
        to_ret.csr = req.substr(lines[3] + 1);
    }
    return to_ret;
}
