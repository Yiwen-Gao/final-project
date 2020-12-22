#ifndef __CLIENTTOOLS_H_
#define __CLIENTTOOLS_H_

#include <memory>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>

#include <openssl/bio.h>
#include <openssl/err.h>

namespace my{

void print_errors_and_exit(const char *message);
void print_errors_and_throw(const char *message);
std::string receive_some_data(BIO *bio);
std::vector<std::string> split_headers(const std::string& text);
std::string receive_http_message(BIO *bio);
void send_http_request(BIO *bio, const std::string& line, const std::string& host);

}

#endif
