#!/bin/bash
cd ..
rm -rf mailsystem

#set up the mail system
echo "Setting up mail system"
./setupmailsystem.sh mailsystem > testscripts/setupout.txt 2>&1
cd mailsystem/server

# run the server in the backgrouund
echo "Starting server"
sudo ./server > ../../testscripts/serveroutput.txt 2>&1 &

#run getcert in the client
echo "Running getcert - enter grovelling_turtle when prompted"
cd ../client
./bin/getcert muermo


