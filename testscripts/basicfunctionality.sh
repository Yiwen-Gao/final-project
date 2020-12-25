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
echo "Running getcert for muermo - enter grovelling_turtle when prompted"
cd ../client
./bin/getcert muermo > ../../testscripts/clientoutput.txt 2>&1 

echo "Running getcert for repine - enter tallyho's_courage when prompted"
./bin/getcert repine > ../../testscripts/clientoutput.txt 2>&1

echo "Running getcert for addleness - enter Cardin_pwns when prompted"
./bin/getcert addleness > ../../testscripts/clientoutput.txt 2>&1

#here would want to verify that it ran correctly?
echo "Running sendmsg"
./bin/sendmsg muermo < ../../testscripts/basicsendmsginput.txt >> ../../testscripts/clientoutput.txt 2>&1

echo "Running recvmsg"
./bin/recvmsg addleness >> ../../testscripts/clientoutput.txt 2>&1

echo "Running changepw"
./bin/changepw muermo > ../../testscripts/clientoutput.txt 2>&1
