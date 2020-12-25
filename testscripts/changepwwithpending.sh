#!/bin/bash
cd ..
rm -rf mailsystem

echo "Testing if changepw rejects if a user has pending mail"
echo "Setting up mail system"
./setupmailsystem.sh mailsystem > testscripts/setupout.txt 2>&1
cd mailsystem/server

# run the server in the background
echo "Starting server"
sudo ./server > ../../testscripts/serveroutput.txt 2>&1 &

#run getcert in the client
echo "Running getcert for muermo - enter grovelling_turtle when prompted"
./bin/getcert muermo > ../../testscripts/clientoutput.txt 2>&1

o "Running getcert for repine - enter tallyho's_courage when prompted"
./bin/getcert repine >> ../../testscripts/clientoutput.txt 2>&1

echo "Running getcert for addleness - enter Cardin_pwns when prompted"
./bin/getcert addleness >> ../../testscripts/clientoutput.txt 2>&1

echo "Running sendmsg"
./bin/sendmsg muermo < ../../testscripts/basicsendmsginput.txt >> ../../testscripts/clientoutput.txt 2>&1

echo "addleness trying to changepw despite having a pending message. Enter Cardin_pwns then newpassword when prompted"
./bin/changepw addleness >> ../../testscript/clientoutput.txt 2>&1
