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
}

Connection::~Connection() {
    close(sock);
    SSL_CTX_free(ctx);
    EVP_cleanup();
}

int Connection::send_string(string to_send) {
    return SSL_write(ssl, to_send.c_str(), to_send.length());
}

int Connection::send_bytes(char *bytes, int num_bytes)
{
    return SSL_write(ssl, bytes, num_bytes);
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
    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL); // SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, verify_callback);
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

string Connection::recv() {
    string msg = "";
    int ilen;

    while ((ilen = SSL_read(ssl, ibuf, sizeof ibuf - 1)) > 0) {
        ibuf[ilen] = '\0';
        printf("%s", ibuf);
        msg += ibuf;
        if (msg.substr( msg.length() - 2 ) == "\n\n") {
            break;
        }
    }

    return msg;
}

vector<char *> Connection::get_sendmsg_messages(int num_messages, vector<int> &sizes) {
    char *msg;
    int ilen, ind, size;
    size_t read;
    vector<char *> messages;
    for (ind = 0; ind < num_messages; ++ind)
    {
        if (!(ilen = SSL_read(ssl, &size, sizeof(int)) > 0))
        {
            messages.push_back("");
            return messages;
        }
        sizes.push_back(size);
        msg = (char *) malloc(size);
        if (!(ilen = SSL_read(ssl, msg, size) > 0))
        {
            messages.push_back(NULL);
            free(msg);
            return messages;
        }
        messages.push_back(msg);
    }
    return messages;
}

void Connection::send(string msg) {
    SSL_write(ssl, msg.c_str(), msg.length());
}
