#!/bin/bash
# Remove the previous ca directory
rm -rf ca

# Begin ca setup, make ca directory in current directory
mkdir ca

# Create the configuration file
cp openssl.cnf ca/openssl.cnf

cd ca
mkdir certs crl newcerts private
chmod 700 private
touch index.txt
echo 1000 > serial

# Create the root key
openssl genrsa -aes256 -passout pass:password123 -out private/ca.key.pem 4096

# User will be prompted for a password

chmod 400 private/ca.key.pem

# Create the root certificate
openssl req -config openssl.cnf \
     -key private/ca.key.pem \
     -new -x509 -days 7300 -sha256 -extensions v3_ca \
     -passout pass:password123 \
	 -passin pass:password123 \
	 -out certs/ca.cert.pem

# The user will enter information to be used in the root certificate

chmod 444 certs/ca.cert.pem

# Verify the root certificate
openssl x509 -noout -text -in certs/ca.cert.pem

# Output will show:
# -Signature algorithm used
# -dates of validity
# -public-key bitlength
# -issuer = entity that signed the certificate
# -subject = certificate itself

# -------------------------------------------------------------

# Create the intermediate pair
mkdir intermediate

cd intermediate
mkdir certs crl csr newcerts private
chmod 700 private
touch index.txt
echo 1000 > serial

# crlnumber file for keeping track of certificate revocation lists
echo 1000 > crlnumber

# Create the configuration file for the intermediate CA
cp ../../intermediateopenssl.cnf openssl.cnf

# Create the intermediate key
cd ..
openssl genrsa -aes256 -passout pass:password123 \
	-out intermediate/private/intermediate.key.pem 4096
chmod 400 intermediate/private/intermediate.key.pem

# Create the intermediate certificate request
openssl req -config intermediate/openssl.cnf -new -sha256 \
	  -passin pass:password123 \
	  -passout pass:password123 \
      -key intermediate/private/intermediate.key.pem \
      -out intermediate/csr/intermediate.csr.pem

# User will be prompted, common name must be different
# Sign the certificate with the root CA
openssl ca -config openssl.cnf -extensions v3_intermediate_ca \
     -days 3650 -notext -md sha256 \
	 -passin pass:password123 \
     -in intermediate/csr/intermediate.csr.pem \
     -out intermediate/certs/intermediate.cert.pem

chmod 444 intermediate/certs/intermediate.cert.pem

# Verify the intermediate certificate

openssl x509 -noout -text \
     -in intermediate/certs/intermediate.cert.pem

openssl verify -CAfile certs/ca.cert.pem \
      intermediate/certs/intermediate.cert.pem

# Should output OK

# Create the certificate chain file
cat intermediate/certs/intermediate.cert.pem \
     certs/ca.cert.pem > intermediate/certs/ca-chain.cert.pem

chmod 444 intermediate/certs/ca-chain.cert.pem
