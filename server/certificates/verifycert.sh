#!/bin/bash

# Verify the certificate
openssl x509 -noout -text \
		-in intermediate/certs/$1.cert.pem