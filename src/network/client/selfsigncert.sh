#!/bin/bash

if [ -z "$1" ]; then
		echo "Must supply username as command line argument"
		exit 0
fi

if [ -z "$2" ]; then
		echo "Must supply a password"
		exit 0
fi

#cd csr

#openssl req -config openssl.cnf -new -sha256 \
#	-passin pass:$2 \
#	-passout pass:$2 \
#	-key private/temp.key.pem \
#	-out temp.csr.pem

#openssl ca -config openssl.cnf -extensions v3_intermediate_ca \
#	-days 3650 -notext -md sha256 \
#	-passin pass:$2 \
#	-in temp.csr.pem \
#	-out ../certificates/$1-selfsignedcert.cert.pem

openssl req -passout pass:$2 \
	-passin pass:$2 \
    -key csr/private/$1.key.pem -sha256\
	-new -sha256 -out csr/$1.selfsignedcsr.pem

openssl x509 -req -in csr/$1.selfsignedcsr.pem \
	-passin pass:$2 \
	-CA certificates/$1.cert.pem \
	-CAkey csr/private/$1.key.pem \
	-CAcreateserial -out certificates/$1.selfsignedcert.pem \
	-CAcreateserial \
	-days 500

#openssl ca -config openssl.cnf \
#	-passin pass:$2 \
#	-extensions encryption_cert -days 375 -notext -md sha256 \
#	-in $1.selfsignedcsr.pem \
#	-out ../certificates/$1.selfsignedcert.pem



#chmod 444 certificates/$1.selfsignedcert.pem
#openssl x509 -noout -text \
#		-in certificates/$1.selfsignedcert.pem

# If we need to modify the chain:
# cat certs/$1.cert.pem > trusted_certs/ca-chain.cert.pem
