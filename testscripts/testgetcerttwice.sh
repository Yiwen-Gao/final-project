#!/bin/bash
# Tests assume mail system exists and server is running in the background

if [ $# -eq 0 ]
    then 
	echo "usage: ./setuppriv.sh <mail_system_name>"
	exit
fi

cd ../$1/client
echo "Running getcert for muermo. Enter grovelling_turtle when prompted for a password"
./bin/getcert muermo

echo "Running getcrert for muermo again. Enter grovelling_turtle"
./bin/getcert muermo
