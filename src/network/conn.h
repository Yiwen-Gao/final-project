#ifndef CONN_H
#define CONN_H

/*************************** HEADER FILES ***************************/
#include <algorithm>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>
extern "C" {
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <stdio.h>
    #include <string.h>
    #include <strings.h>
    #include <sys/socket.h>
    #include <sys/types.h>
    #include <sys/wait.h>
    #include <unistd.h>

    #include <openssl/ssl.h>
    #include <openssl/bio.h>
    #include <openssl/err.h>
    #include <openssl/x509.h>
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
        int err; 

        char ibuf[INPUT_BUFFER_SIZE];
        char obuf[OUTPUT_BUFFER_SIZE];

        struct sockaddr_in sin;
        int sock;
        struct hostent *he;

        void set_sock();

    public:
        Connection(const char *ca_cert, const char *my_cert, const char *my_key);
        ~Connection();

        void set_certs();
        void set_bio();
        std::string recv();
        void send(std::string msg);
};

class ClientConnection : public Connection {
    public:
        ClientConnection(const char *ca_cert, const char *my_cert, const char *my_key);
        void set_sock();
        void connect_server();
};

struct REQ {
    std::string user;
    std::string password;
    std::string csr;
    std::string type;
};

class ServerConnection : public Connection {
    int client;
    
    public:
        ServerConnection(const char *ca_cert, const char *my_cert, const char *my_key);
        void set_sock();
        int accept_client();
        int send_string(std::string to_send);
        REQ parse_req(std::string msg);
};

#endif
