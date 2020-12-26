#!/bin/bash

if [ $# -eq 0 ]
    then 
	echo "usage: ./setuppriv.sh <mail_system_name>"
	exit
fi

cd ../$1/client
echo "Running getcert for analects. Enter anything besides pickerel_symbiosis when prompted for a password"
./bin/getcert analects
