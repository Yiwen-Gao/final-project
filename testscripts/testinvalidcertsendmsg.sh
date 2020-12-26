#!/bin/bash

if [ $# -eq 0 ]
    then 
	echo "usage: ./setuppriv.sh <mail_system_name>"
	exit
fi

cd ../$1/client
echo "Running getcert for whaledom. Enter petering_sounding's when prompted for a password"
./bin/getcert whaledom

echo "Running getcert for wamara. Enter stirrer_hewer's when prompted"
./bin/getcert wamara
rm certificates/wamara.cert.pem
touch certificates/wamara.cert.pem

echo "Running sendmsg to a user signing with a garbage certificate. Should be unable to decrypt"
./bin/sendmsg wamara < ../../testscripts/sendmsginput.txt 
