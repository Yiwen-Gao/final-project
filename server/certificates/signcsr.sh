#!/bin/bash

# Will create a certificate associated with a identifier
# Must be supplied the idenfifier as a command line argumnent.

if [ -z  "$1" ]; then
	echo "Must supply a unique identifier as a command line argument"
	exit 0
fi

# # Create a key for the client cert
# cd ca
# openssl genrsa -aes256 \
# 		-passout pass:password123 \
# 		-out intermediate/private/$1.key.pem 2048
# chmod 400 intermediate/private/$1.key.pem

# Create the certificate
# Certificate signing request. Common Name must be domain name

# openssl req -config intermediate/openssl.cnf \
# 		    -passout pass:password123 \
# 			-passin pass:password123 \
# 			-key intermediate/private/$1.key.pem \
# 			-new -sha256 -out intermediate/csr/$1.csr.pem

openssl ca -config $1openssl.cnf \
		   -extensions v3_intermediate_ca -days 375 -notext -md sha256 \
		   -in $1csr/$2.csr.pem \
	  	   -out $1certs/$2.cert.pem \
		   -passin pass:password123

chmod 640 $1certs/$2.cert.pem

# Verify the certificate
openssl x509 -noout -text \
		-in $1certs/$2.cert.pem
