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

namespace my {
my::StringBIO::StringBIO() {
	methods_.reset(BIO_meth_new(BIO_TYPE_SOURCE_SINK, "StringBIO"));
	if (methods_ == nullptr) {
			throw std::runtime_error("StringBIO: error in BIO_meth_new");
	}
	BIO_meth_set_write(methods_.get(), [](BIO *bio, const char *data, int len) -> int {
		std::string *str = reinterpret_cast<std::string*>(BIO_get_data(bio));
		str->append(data, len);
		return len;
	});
	bio_.reset(BIO_new(methods_.get()));
	if (bio_ == nullptr) {
		throw std::runtime_error("StringBIO: error in BIO_new");
	}
	BIO_set_data(bio_.get(), &str_);
	BIO_set_init(bio_.get(), 1);
}
	
}
