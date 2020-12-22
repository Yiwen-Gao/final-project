#ifndef __STRINGBIO_H__
#define __STRINGBIO_H__

#include <memory>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>

#include <openssl/bio.h>
#include <openssl/err.h>


namespace my {

template<class T> struct DeleterOf;
template<> struct DeleterOf<BIO> { void operator()(BIO *p) const { BIO_free_all(p); } };
template<> struct DeleterOf<BIO_METHOD> { void operator()(BIO_METHOD *p) const { BIO_meth_free(p); } };

template<class OpenSSLType>
using UniquePtr = std::unique_ptr<OpenSSLType, DeleterOf<OpenSSLType>>;

class StringBIO {
	std::string str_;
	my::UniquePtr<BIO_METHOD> methods_;
	my::UniquePtr<BIO> bio_;

public:
	StringBIO(StringBIO&&) = delete;
	StringBIO& operator=(StringBIO&&) = delete;

	explicit StringBIO();
	
	BIO *bio() { return bio_.get(); }
	std::string str() && { return std::move(str_); }

};

} //namespace my

#endif
