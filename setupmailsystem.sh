#!/bin/bash

if [ $# -eq 0 ]
    then 
	echo "Must supply a name for the mail system"
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
cd ca
mkdir intermediate root users bin
cd ../password
mkdir bin
touch passwords.txt
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

cd ../mail
make

cd ../network
make

cd client
make
cp getcert changepw sendmsg recvmsg ../../../$1/client/bin

cd ../../passwords
make


