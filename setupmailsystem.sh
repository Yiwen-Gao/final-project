#!/bin/bash

if [ $# -eq 0 ]
    then 
	echo "usage: ./setupmailsystem.sh <mail_system_name>"
	exit
fi

if [ -d $1 ]
then
    echo "${1} already exists"
    exit 1
fi

mkdir "$1"
cd "$1"
mkdir server client

cd server
mkdir ca password mail
cd password
mkdir bin
touch passwords.txt
cd ../ca
mkdir bin
cd ../mail
mkdir bin mail
cd mail

mkdir addleness
mkdir analects
mkdir annalistic
mkdir anthropomorphologically
mkdir blepharosphincterectomy
mkdir corector
mkdir durwaun
mkdir dysphasia
mkdir encampment
mkdir endoscopic
mkdir exilic
mkdir forfend
mkdir gorbellied
mkdir gushiness
mkdir muermo
mkdir neckar
mkdir outmate
mkdir outroll
mkdir overrich
mkdir philosophicotheological
mkdir pockwood
mkdir polypose
mkdir refluxed
mkdir reinsure
mkdir repine
mkdir scerne
mkdir starshine
mkdir unauthoritativeness
mkdir unminced
mkdir unrosed
mkdir untranquil
mkdir urushinic
mkdir vegetocarbonaceous
mkdir wamara
mkdir whaledom

cd ../../../client
mkdir csr certs bin dummy

#make everything and copy it into the correct locations
cd ../../src/certificates
make
cp get-cert ../../$1/server/ca/bin

cd ../mail
make
cp get-msg ../../$1/server/mail/bin

cd ../server
make
cp server ../../$1/server

cd client
make
cp getcert changepw sendmsg recvmsg createcsr intermediateopenssl.cnf selfsigncert.sh ../../../$1/client/bin

cd ../../passwords
make
cp crypt-pw verify-pw add-user change-pw ../../$1/server/password/bin

#copy the ca stuff into the structure
cd ../../server/certificates
cp setupca.sh issueservercert.sh issueclientcert.sh signcsr.sh ../../$1/server/ca/bin
cp intermediateopenssl.cnf openssl.cnf casetupinput.txt serverinput.txt dummyinput.txt ../../$1/server/ca

#setup the ca within the ca directory
cd ../../$1/server/ca
./bin/setupca.sh < casetupinput.txt

#issue the server certificate
./bin/issueservercert.sh localhost < serverinput.txt

#issue the dummy certificate and copy into the appropriate location
./bin/issueclientcert.sh dummy < dummyinput.txt
cp ./ca/intermediate/certs/dummy.cert.pem ./ca/intermediate/private/dummy.key.pem ../../client/dummy
