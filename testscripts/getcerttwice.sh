#!/bin/bash
cd ..
rm -rf mailsystem

#set up the mail system
echo "Setting up mail system"
./setupmailsystem.sh mailsystem > testscripts/setupout.txt 2>&1
cd mailsystem/server

# run the server in the background
echo "Starting server"
sudo ./server > ../../testscripts/serveroutput.txt 2>&1 &

#run getcert in the client
echo "Running getcert for muermo - enter grovelling_turtle when prompted"
cd ../client
./bin/getcert muermo > ../../testscripts/clientoutput.txt 2>&1

#run getcert in the client again
echo "Running getcert for muermo again - enter grovelling_turtle when prompted"
cd ../client
./bin/getcert muermo >> ../../testscripts/clientoutput.txt 2>&1

