#!/bin/bash

# Will create a certificate associated with a URL
# Must be supplied the URL as a command line argumnent.

if [ -z  "$1" ]; then
	echo "Must supply a url as a command line argument"
	exit 0
fi

# Create a key for the server cert
# Note: can't use passphrase because incompatible with SSL C code
cd ca
openssl genrsa -out intermediate/private/$1.key.pem 2048

chmod 400 intermediate/private/$1.key.pem

# Create the certificate
# Certificate signing request. Common Name must be domain name

openssl req -config intermediate/openssl.cnf \
	-passin pass:password123 \
	-key intermediate/private/$1.key.pem \
	-new -sha256 -out intermediate/csr/$1.csr.pem

openssl ca -config intermediate/openssl.cnf \
	-passin pass:password123 \
	-extensions usr_cert -days 375 -notext -md sha256 \
	-in intermediate/csr/$1.csr.pem \
	-out intermediate/certs/$1.cert.pem

chmod 444 intermediate/certs/$1.cert.pem

# Verify the certificate
openssl x509 -noout -text \
	-in intermediate/certs/$1.cert.pem
