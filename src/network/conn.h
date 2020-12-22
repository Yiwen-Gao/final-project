#ifndef CONN_H
#define CONN_H

/*************************** HEADER FILES ***************************/
#include <iostream>
extern "C" {
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <stdio.h>
    #include <string.h>
    #include <strings.h>
    #include <sys/socket.h>
    #include <sys/types.h>
    #include <unistd.h>

    #include <openssl/ssl.h>
    #include <openssl/bio.h>
    #include <openssl/err.h>
}

/****************************** MACROS ******************************/
#define HOST_NAME "localhost"
#define DEFAULT_PORT 443
#define INPUT_BUFFER_SIZE 512
#define OUTPUT_BUFFER_SIZE 512

/*********************** CLASS DECLARATIONS **********************/
class Connection {
    protected:
        const char *ca_cert;
        const char *my_cert;
        const char *my_key;

        SSL_CTX *ctx;
        SSL *ssl;
        const SSL_METHOD *meth;
        BIO *sbio;
        int err; char *s;

        char ibuf[INPUT_BUFFER_SIZE];
        char obuf[OUTPUT_BUFFER_SIZE];

        struct sockaddr_in sin;
        int sock;

        // client
        struct hostent *he;
        // server
        int client;

        void set_sock();

    public:
        Connection(const char *ca_cert, const char *my_cert, const char *my_key);
        ~Connection();

        void set_certs();
        void set_bio();
        bool write();
        bool read();
};

class ClientConnection : public Connection {
    public:
        ClientConnection(const char *ca_cert, const char *my_cert, const char *my_key);
        void set_sock();
        void connect_server();
};

class ServerConnection : public Connection {
    public:
        ServerConnection(const char *ca_cert, const char *my_cert, const char *my_key);
        void set_sock();
        void accept_client();
};

#endif