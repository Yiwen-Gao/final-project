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

cd ./ca/ca

yes | openssl ca -config ./intermediate/openssl.cnf \
		   -extensions usr_cert -days 375 -notext -md sha256 \
		   -in ./intermediate/csr/$1.csr.pem \
	  	   -out ./intermediate/certs/$1.cert.pem \
		   -passin pass:password123

chmod 640 $1certs/$2.cert.pem

# Verify the certificate
openssl x509 -noout -text \
		-in ./intermediate/certs/$1.cert.pem
