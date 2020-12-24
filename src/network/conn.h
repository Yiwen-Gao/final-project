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

#define GET_CERT "getcert"
#define CHANGE_PW "changepw"
#define SEND_MSG "sendmsg"
#define RECV_MSG "recvmsg"

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
        std::vector<char *> get_sendmsg_messages(int num_messages, std::vector<int> &sizes);
};

class ClientConnection : public Connection {
    public:
        ClientConnection(const char *ca_cert, const char *my_cert, const char *my_key);
        ~ClientConnection();
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
        int send_bytes(char *bytes, int num_bytes);
        std::string get_common_name();
        void close_client();
        int send_string(std::string to_send);
        REQ parse_req(std::string req);
};

// base req abstract class
struct BaseReq {
    std::string type;
    virtual std::string get_header();
    virtual std::string get_body();
    std::string get_http_content() {
        return this->get_header() + this->get_body() + "\n";
    }
};

struct GetCertReq : public BaseReq {
    std::string username;
    std::string password;
    std::string csr;
    GetCertReq(std::string username, std::string password, std::string csr);
    GetCertReq(std::string content);
    std::string get_header();
    std::string get_body();
};

struct ChangePWReq : public BaseReq {
    std::string username;
    std::string old_password;
    std::string new_password;
    std::string csr;
    ChangePWReq(std::string username, std::string old_password, std::string new_password, std::string csr);
    ChangePWReq(std::string content);
    std::string get_header();
    std::string get_body();
};

struct SendMsgReq : public BaseReq {
    std::vector<std::string> usernames;
    SendMsgReq(std::vector<std::string> usernames);
    SendMsgReq(std::string usernames);
    std::string get_header();
    std::string get_body();
};

// struct SendMsgMailReq : public BaseReq {
//     std::vector<std::string> msgs;
//     SendMsgMailReq(std::vector<std::string> msgs);
//     SendMsgMailReq(std::string msgs);
//     std::string get_header();
//     std::string get_body();
// };

struct RecvMsgReq : public BaseReq {
    std::string username;
    RecvMsgReq(std::string username);
    std::string get_header();
    std::string get_body();
};

struct BaseResp {
    std::string type;
    virtual std::string get_header();
    virtual std::string get_body();
    std::string get_http_content() {
        return this->get_header() + "\n" + this->get_body() + "\n";
    }
};

struct CertResp : public BaseResp {
    std::string cert;
    CertResp(std::string cert);
    std::string get_body();
};

struct MailCertResp : public BaseResp {
    std::vector<std::string> certs;
    MailCertResp(std::vector<std::string> certs);
    MailCertResp(std::string content);
    std::string get_body();
};

struct MailResp : public BaseResp {
    std::string address;
    std::string msg;
    MailResp(std::string content);
    std::string get_body();
};

/*********************** FUNCTION DECLARATIONS **********************/
BaseReq *parse_req(std::string &http_content);

std::string vec_to_str(std::vector<std::string> &vec);
std::vector<std::string> str_to_vec(std::string &str);
std::string remove_headers(std::string &http_content);

#endif
