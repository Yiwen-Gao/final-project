#include <memory>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>

#include <openssl/bio.h>
#include <openssl/err.h>


#include "StringBIO.h"
#include "clienttools.h"


namespace my {

[[noreturn]] void print_errors_and_exit(const char *message)
{
	fprintf(stderr, "%s\n", message);
	ERR_print_errors_fp(stderr);
	exit(1);
}

[[noreturn]] void print_errors_and_throw(const char *message)
{
	StringBIO bio;
	ERR_print_errors(bio.bio());
	throw std::runtime_error(std::string(message) + "\n" + std::move(bio).str());
}

std::string receive_some_data(BIO *bio)
{
	char buffer[1024];
	int len = BIO_read(bio, buffer, sizeof(buffer));
	if (len < 0) {
		my::print_errors_and_throw("error in BIO_read");
	} else if (len > 0)
	{
		return std::string(buffer, len);
	} else if (BIO_should_retry(bio)) {
		return receive_some_data(bio);
	} else {
		my::print_errors_and_throw("empty BIO_read");
	}
}

std::vector<std::string> split_headers(const std::string& text)
{
	std::vector<std::string> lines;
	const char *start = text.c_str();
	while (const char *end = strstr(start, "\r\n")) {
		lines.push_back(std::string(start, end));
		start = end + 2;
	}
	return lines;
}

std::string receive_http_message(BIO *bio)
{
	std::string headers = my::receive_some_data(bio);
	char *end_of_headers = strstr(&headers[0], "\r\n\r\n");
	while (end_of_headers == nullptr) {
		headers += my::receive_some_data(bio);
		end_of_headers = strstr(&headers[0], "\r\n\r\n");
	}
	
	std::string body = std::string(end_of_headers+4, &headers[headers.size()]);
	headers.resize(end_of_headers + 2 - &headers[0]);
	size_t content_length = 0;
	for (const std::string& line : my::split_headers(headers)) {
		if (const char *colon = strchr(line.c_str(), ':')) {
			auto header_name = std::string(&line[0], colon);
			if (header_name == "Content-Length") {
				content_length = std::stoul(colon+1);
			}
		}
	}
	while (body.size() < content_length) {
		body += my::receive_some_data(bio);
	}
	return headers + "\r\n" + body;
}

void send_http_request(BIO *bio, const std::string& line, const std::string& host)
{
	std::string request = line + "\r\n";
	request += "Host: " + host + "\r\n";
	request += "\r\n";

	BIO_write(bio, request.data(), request.size());
	BIO_flush(bio);
}

}//namespace my

