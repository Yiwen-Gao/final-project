#!/bin/bash
cd ..
rm -rf mailsystem

echo "Setting up mail system"
./setupmailsystem.sh mailsystem > testscripts/setupout.txt 2>&1
cd mailsystem/server

#test with an incorrect password
echo "Running getcert for muermo - enter fakepassword when prompted"
cd ../client
./bin/getcert muermo > ../../testscripts/clientoutput.txt 2>&1
