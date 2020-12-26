#!/bin/bash

if [ $# -eq 0 ]
    then 
	echo "usage: ./setupmailsystem.sh <mail_system_name>"
	exit
fi

rm -rf $1

if [ -d $1 ]
then
    echo "${1} already exists"
    exit 1
fi

# make folders
mkdir "$1"
cd "$1"
mkdir server client

# certificates
cd ../src/certificates
make
cd ../../

# server - ca
mkdir $1/server/ca $1/server/ca/bin
cp src/certificates/{intermediateopenssl.cnf,openssl.cnf} $1/server/ca
cp src/certificates/{issueservercert.sh,get-cert,setupca.sh,signcsr.sh} $1/server/ca/bin

# issue the dummy certificate and copy into the appropriate location
$1/server/ca/bin/issueservercert.sh dummy < src/certificates/dummyinput.txt
# cp ./ca/intermediate/certs/dummy.cert.pem ./ca/intermediate/private/dummy.key.pem ../../client/dummy
# setup the ca within the ca directory
mkdir $1/server/ca/ca
$1/server/bin/setupca.sh < src/certificates/casetupinput.txt

# server - mail
cd src/mail
./create-tree temp
make TREE=temp

cd ../../
mkdir $1/server/mail 
cp -r src/mail/temp/* $1/server/mail 

# server - passwords
cd src/passwords
make
./crypt-pw 

cd ../../
mkdir $1/server/passwords $1/server/passwords/bin
cp src/passwords/passwords.txt $1/server/passwords
cp src/passwords/{add-user,change-pw,verify-pw} $1/server/passwords/bin

# server - main program
cd src/server
make

cd ../../
cp src/server/server $1/server

# client - main programs
cd src/client
make

cd ../../
mkdir $1/client/bin 
cp src/client/{getcert,changepw,createcsr,sendmsg,recvmsg} $1/client/bin

# client - certs
mkdir $1/client/{certs,csr,dummy,trusted_certs} $1/client/csr/private