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
mkdir $1/server/ca $1/server/ca/bin $1/server/ca/ca
cp src/certificates/{casetupinput.txt,dummyinput.txt,intermediateopenssl.cnf,openssl.cnf,serverinput.txt} $1/server/ca
cp src/certificates/{issueclientcert.sh,issueservercert.sh,get-cert,setupca.sh,signcsr.sh} $1/server/ca/bin

cd $1/server/ca
# setup the ca within the ca directory
./bin/setupca.sh < casetupinput.txt
# issue server cert
./bin/issueservercert.sh localhost < serverinput.txt
# # issue client cert
./bin/issueclientcert.sh dummy < dummyinput.txt
cd ../../../

# server - mail
# create mailboxes
mkdir $1/server/mail 
cd $1/server/mail
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

cd ../../../src/mail
make

cd ../../
cp -r src/mail/get-msg $1/server/mail/get-msg

# server - passwords
cd src/passwords
make
./crypt-pw 

cd ../../
mkdir $1/server/password $1/server/password/bin
cp src/password/passwords.txt $1/server/password
cp src/password/{add-user,change-pw,verify-pw} $1/server/password/bin

# server - main program
cd src/server
make

cd ../../
cp src/server/server $1/server/ca/bin

# client - certs
mkdir $1/client/{certs,csr,dummy,trusted_certs} $1/client/csr/private
cd $1/server/ca
# copy the trusted certs from server to client
cp ./ca/certs/ca.cert.pem ./ca/intermediate/certs/{intermediate.cert.pem,ca-chain.cert.pem} ../../client/trusted_certs
# copy dummy info from server to client
cp ./ca/intermediate/certs/dummy.cert.pem ../../client/dummy/cert.pem
cp ./ca/intermediate/private/dummy.key.pem ../../client/dummy/key.pem
cd ../../../

# client - main program
cd src/client
make

cd ../../
mkdir $1/client/bin
cp src/client/{changepw,createcsr,getcert,recvmsg,sendmsg} $1/client/bin
